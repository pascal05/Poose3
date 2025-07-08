#include <iostream>
#include "Server.hpp"

int main()
{
    Server server;

    std::vector<std::pair<std::string, std::string>> users = {
        {"john_doe", "password123"},
        {"jane_doe", "password456"},
        {"alice_smith", "password789"},
        {"bob_johnson", "password101"},
        {"charlie_brown", "password112"}};

    std::map<std::string, int> userSessions;

    // Register and signin users
    for (const auto &user : users)
    {
        server.register_user(user.first, user.second);
        int sessionId = server.signin(user.first, user.second);
        userSessions[user.first] = sessionId;
    }

    // Show initial product list (copies)
    std::cout << "\nInitial Product List:\n";
    for (const auto &product : server.products_list(userSessions["john_doe"]))
    {
        std::cout << "Product ID: " << product.getproductid()
                  << ", Name: " << product.getproductname()
                  << ", Price: " << product.getcurrentprice() << std::endl;
    }

    std::cout << "\n--- User Interaction Loop: Buy + Sell ---\n";

    const int num_rounds = 3;
    int product_id = 1;

    for (int round = 0; round < num_rounds; ++round)
    {
        std::cout << "\n-- Round " << round + 1 << " --\n";
        for (const auto &user : users)
        {
            const std::string &username = user.first;
            int sessionId = userSessions[username];

            std::cout << username << " tries to BUY Product ID: " << product_id << std::endl;
            try
            {
                if (server.buy(product_id, sessionId))
                    std::cout << "  -> Buy success\n";
                else
                    std::cout << "  -> Buy failed\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "  -> Buy error: " << e.what() << std::endl;
            }

            // Show inventory after buying
            std::cout << "  Inventory:\n";
            for (const auto &[pid, qty] : server.list_of_own_inventory(sessionId))
            {
                std::cout << "    Product ID: " << pid << ", Quantity: " << qty << std::endl;
            }

            std::cout << username << " tries to SELL Product ID: " << product_id << std::endl;
            try
            {
                if (server.sell(product_id, sessionId))
                    std::cout << "  -> Sell success\n";
                else
                    std::cout << "  -> Sell failed\n";
            }
            catch (const std::exception &e)
            {
                std::cout << "  -> Sell error: " << e.what() << std::endl;
            }

            // Show inventory after selling
            std::cout << "  Inventory after selling:\n";
            for (const auto &[pid, qty] : server.list_of_own_inventory(sessionId))
            {
                std::cout << "    Product ID: " << pid << ", Quantity: " << qty << std::endl;
            }

            // Cycle product id 1-5
            product_id = (product_id % 5) + 1;
        }
    }

    // Show final wallet balances
    std::cout << "\n--- Final Wallet Balances ---\n";
    for (const auto &[username, sessionId] : userSessions)
    {
        std::cout << username << ": " << server.wallet(sessionId) << std::endl;
    }

    // Show price history for products 1 to 5 (work with copies)
    std::cout << "\n--- Price History for Products 1-5 ---\n";
    for (int pid = 1; pid <= 5; ++pid)
    {
        const Product* product_ptr = nullptr;
        auto products = server.products_list(userSessions["john_doe"]); // get fresh copies each time
        for (const auto &p : products)
        {
            if (p.getproductid() == pid)
            {
                product_ptr = &p;
                break;
            }
        }

        if (!product_ptr)
        {
            std::cout << "Product ID " << pid << " not found.\n";
            continue;
        }

        std::cout << "Product ID " << pid << " Price History:\n";
        for (const auto &[timestamp, price] : product_ptr->getpricehistory())
        {
            std::cout << "  Timestamp: " << timestamp << ", Price: " << price << std::endl;
        }
    }

    return 0;
}
