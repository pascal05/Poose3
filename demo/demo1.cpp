#include <iostream>
#include "Server.hpp"

int main() {
    Server server;

    // Register and sign in user
    server.register_user("user1", "pass1");
    int sessionId = server.signin("user1", "pass1");
    if (sessionId == -1) {
        std::cerr << "Sign-in failed\n";
        return 1;
    }

    // Get the list of products (copies)
    auto products = server.products_list(sessionId);

    // Find product 1 by value
    Product* product1_ptr = nullptr;
    for (auto& p : products) {
        if (p.getproductid() == 1) {
            product1_ptr = &p;
            break;
        }
    }

    if (!product1_ptr) {
        std::cerr << "Product 1 not found\n";
        return 1;
    }

    std::cout << "Initial price product 1: " << product1_ptr->getcurrentprice() << "\n";

    // Buy product 1 multiple times to update price multiple times
    for (int i = 0; i < 5; ++i) {
        bool success = server.buy(1, sessionId);
        if (!success) {
            std::cerr << "Buy attempt " << i << " failed\n";
        }
    }

    // Since product1_ptr points to a copy, to get updated data, fetch products again:
    products = server.products_list(sessionId);
    Product* updated_product1 = nullptr;
    for (auto& p : products) {
        if (p.getproductid() == 1) {
            updated_product1 = &p;
            break;
        }
    }
    if (!updated_product1) {
        std::cerr << "Product 1 not found after buys\n";
        return 1;
    }

    // Print all price history entries for product 1 after buys
    std::cout << "\nPrice history for product 1 after multiple buys:\n";
    const auto& history = updated_product1->getpricehistory();
    for (const auto& [timestamp, price] : history) {
        std::cout << "  Timestamp: " << timestamp << ", Price: " << price << "\n";
    }

    return 0;
}
