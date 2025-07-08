#include "Server.hpp"
#include <random>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <iostream>

Server::Server() : lastsessionid(0) {
    // Initialize products
    std::vector<Product> initial_products = {
        Product(1, "Pepper", 0.05f),
        Product(2, "Bread", 1.5f),
        Product(3, "Cheese", 3.0f),
        Product(4, "Whool", 25.0f),
        Product(5, "Pig", 150.0f),
        Product(6, "Knive", 20.0f),
        Product(7, "Hammer", 35.0f),
        Product(8, "Shoe", 50.0f),
        Product(9, "Fish", 4.5f),
        Product(10, "Candles", 9.5f)
    };

    for (const auto &product : initial_products) {
        products.emplace(product.getproductid(), product);
    }


}



int Server::signin(const std::string &username, const std::string &password) {
    if (username.empty() || password.empty()) {
        throw std::runtime_error("Username and password cannot be empty");
    }

    for (auto &pair : registeredplayers) {
        User &user = pair.second;
        if (user.getusername() == username && user.getpassword() == password) {
            if (user.getaktivesessionid() != -1) {
                throw std::runtime_error("User already logged in");
            }

            int new_session_id = ++lastsessionid;
            auto it = registeredplayers.find(pair.first);
            if (it != registeredplayers.end()) {
                it->second.setaktivesessionid(new_session_id);
            }

            {
                aktive_session_ids.insert(new_session_id);
                session_last_active[new_session_id] = std::chrono::steady_clock::now();
            }

            std::cout << "User " << username << " signed in successfully with session ID " << new_session_id << "." << std::endl;
            return new_session_id;
        }
    }

    return -1;
}

bool Server::register_user(const std::string &username, const std::string &password) {
    for (const auto &pair : registeredplayers) {
        if (pair.second.getusername() == username) {
            return false;
        }
    }

    if (username.empty() || password.empty()) {
        throw std::runtime_error("Username and password cannot be empty");
    }

    int new_user_id = registeredplayers.size() + 1;
    User new_user(new_user_id, username, password);
    registeredplayers.emplace(new_user_id, std::move(new_user));

    std::cout << "User " << username << " registered successfully with ID " << new_user_id << "." << std::endl;
    return true;
}

void Server::signout(int session_id) {
    {
        
        if (aktive_session_ids.find(session_id) == aktive_session_ids.end()) {
            throw std::runtime_error("Invalid session ID");
        }
        aktive_session_ids.erase(session_id);
        session_last_active.erase(session_id);
    }

    for (auto &pair : registeredplayers) {
        User &user = pair.second;
        if (user.getaktivesessionid() == session_id) {
            user.setaktivesessionid(-1);
            std::cout << "User with ID " << user.getuserID() << " signed out successfully." << std::endl;
            return;
        }
    }

    throw std::runtime_error("Session ID not found");
}

User &Server::getuserbysessionid(int session_id) {


    if (aktive_session_ids.find(session_id) == aktive_session_ids.end()) {
        throw std::runtime_error("Invalid session ID");
    }

    for (auto &pair : registeredplayers) {
        User &user = pair.second;
        if (user.getaktivesessionid() == session_id) {
            return user;
        }
    }

    throw std::runtime_error("Session ID not found");
}

bool Server::checkifsessionidisvalid(int session_id) {

    return aktive_session_ids.find(session_id) != aktive_session_ids.end();
}

std::map<int, int> Server::list_of_own_inventory(int session_id) {
    User &inventoryuser = getuserbysessionid(session_id);
    return inventoryuser.getstock();
}

float Server::wallet(int session_id) {
    User &walletuser = getuserbysessionid(session_id);
    return walletuser.getwallet();
}

bool Server::buy(int product_id, int session_id) {

    User &buyinguser = getuserbysessionid(session_id);
    auto product_it = products.find(product_id);
    if (product_it == products.end()) {
        throw std::runtime_error("Product not found");
    }

    Product &product = product_it->second;
    float product_price = product.getcurrentprice();

    if (buyinguser.getwallet() < product_price) {
        throw std::runtime_error("Insufficient wallet balance");
    }

    std::map<int, int> user_stock = buyinguser.getstock();
    user_stock[product_id]++;
    buyinguser.setstock(user_stock);

    buyinguser.setwallet(buyinguser.getwallet() - product_price);

    float new_price = product_price * random_percent(1.05f, 1.10f);
    product.setcurrentprice(new_price);

    return true;
}

bool Server::sell(int product_id, int session_id) {

    User &sellinguser = getuserbysessionid(session_id);
    auto product_it = products.find(product_id);
    if (product_it == products.end()) {
        throw std::runtime_error("Product not found");
    }

    std::map<int, int> user_stock = sellinguser.getstock();
    auto stock_it = user_stock.find(product_id);
    if (stock_it == user_stock.end() || stock_it->second <= 0) {
        throw std::runtime_error("Product not in inventory");
    }

    user_stock[product_id]--;
    if (user_stock[product_id] == 0) {
        user_stock.erase(stock_it);
    }
    sellinguser.setstock(user_stock);

    float product_price = product_it->second.getcurrentprice();
    sellinguser.setwallet(sellinguser.getwallet() + product_price);

    std::cout << "User with ID " << sellinguser.getuserID() << " sold Product ID: " << product_id << std::endl;
    std::cout << "New wallet balance: " << sellinguser.getwallet() << std::endl;

    float new_price = product_price * random_percent(0.90f, 0.95f);
    product_it->second.setcurrentprice(new_price);

    return true;
}

std::list<Product> Server::products_list(int session_id) {

    if (!checkifsessionidisvalid(session_id)) {
        throw std::runtime_error("Invalid session ID");
    }

    std::list<Product> product_list;
    for (auto &pair : products) {
        product_list.push_back(pair.second);
    }
    return product_list;
}

float Server::product_price(int product_id, int session_id) {

    if (!checkifsessionidisvalid(session_id)) {
        throw std::runtime_error("Invalid session ID");
    }

    auto product_it = products.find(product_id);
    if (product_it == products.end()) {
        throw std::runtime_error("Product not found");
    }

    return product_it->second.getcurrentprice();
}

std::list<std::string> Server::getmessages(int session_id) {

    if (!checkifsessionidisvalid(session_id)) {
        throw std::runtime_error("Invalid session ID");
    }
    return Messages;
}

void Server::sendmessage(int session_id, const std::string &message) {

    if (!checkifsessionidisvalid(session_id)) {
        throw std::runtime_error("Invalid session ID");
    }

    Messages.push_back(message);
    std::cout << "Message added: " << message << std::endl;
}

float Server::random_percent(float min_percent, float max_percent) {
    static std::mt19937 gen(std::random_device{}());
    std::uniform_real_distribution<> dist(min_percent, max_percent);
    return dist(gen);
}
