#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <functional>
#include <list>
#include <set>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include "User.hpp"
#include "Product.hpp"

/**
 * @class Server
 * @brief Handles user management, product transactions, chat messages,
 *        and automatic logout after 3 minutes of inactivity.
 */
class Server
{
private:
    /** Maps player ID to User objects */
    std::map<int, User> registeredplayers;

    /** Maps product ID to Product objects */
    std::map<int, Product> products;

    /** Set of currently active session IDs */
    std::set<int> aktive_session_ids;

    /** Last used session ID, used to generate new session IDs */
    int lastsessionid;

    /** List of chat messages */
    std::list<std::string> Messages;

    /** Maps session ID to last activity timestamp for inactivity tracking */
    std::map<int, std::chrono::steady_clock::time_point> session_last_active;



public:
    /**
     * @brief Constructs the Server and initializes products.
     *        S
     */
    Server();

    /**
     * @brief Signs in a user with the given username and password.
     * @param username The username of the user.
     * @param password The password of the user.
     * @return Session ID if sign-in is successful, otherwise -1.
     * @throws std::runtime_error if username or password is empty, or user already logged in.
     */
    int signin(const std::string &username, const std::string &password);

    /**
     * @brief Registers a new user with username and password.
     * @param username The username for registration.
     * @param password The password for registration.
     * @return True if registration successful, false if username exists.
     * @throws std::runtime_error if username or password is empty.
     */
    bool register_user(const std::string &username, const std::string &password);

    /**
     * @brief Signs out a user by session ID.
     * @param session_id The session ID of the user to log out.
     * @throws std::runtime_error if session ID is invalid or not found.
     */
    void signout(int session_id);

    /**
     * @brief Returns reference to User object by session ID.
     * @param session_id The session ID.
     * @return Reference to User object.
     * @throws std::runtime_error if session ID is invalid or not found.
     */
    User &getuserbysessionid(int session_id);

    /**
     * @brief Checks if a session ID is currently valid.
     * @param session_id The session ID to check.
     * @return True if valid, false otherwise.
     */
    bool checkifsessionidisvalid(int session_id);

    /**
     * @brief Gets the user's inventory (product ID to quantity).
     * @param session_id The user's session ID.
     * @return Map of product IDs to quantities owned.
     * @throws std::runtime_error if session ID invalid.
     */
    std::map<int, int> list_of_own_inventory(int session_id);

    /**
     * @brief Returns the user's current wallet balance.
     * @param session_id The user's session ID.
     * @return Wallet balance as float.
     * @throws std::runtime_error if session ID invalid.
     */
    float wallet(int session_id);

    /**
     * @brief Buys a product and adds it to user's inventory.
     * @param product_id The product ID to buy.
     * @param session_id The user's session ID.
     * @return True if purchase successful.
     * @throws std::runtime_error if product not found or insufficient funds.
     */
    bool buy(int product_id, int session_id);

    /**
     * @brief Sells a product from user's inventory.
     * @param product_id The product ID to sell.
     * @param session_id The user's session ID.
     * @return True if sale successful.
     * @throws std::runtime_error if product not found or not owned.
     */
    bool sell(int product_id, int session_id);

    /**
     * @brief Returns a list of all products.
     * @param session_id The user's session ID.
     * @return List of Product objects.
     * @throws std::runtime_error if session ID invalid.
     */
    std::list<Product> products_list(int session_id);

    /**
     * @brief Gets the current price of a product.
     * @param product_id The product ID.
     * @param session_id The user's session ID.
     * @return Current price as float.
     * @throws std::runtime_error if session ID or product invalid.
     */
    float product_price(int product_id, int session_id);

    /**
     * @brief Returns all chat messages.
     * @param session_id The user's session ID.
     * @return List of chat messages as strings.
     * @throws std::runtime_error if session ID invalid.
     */
    std::list<std::string> getmessages(const int session_id);

    /**
     * @brief Adds a message to the chat.
     * @param session_id The user's session ID.
     * @param message The message string to add.
     * @throws std::runtime_error if session ID invalid.
     */
    void sendmessage(const int session_id, const std::string &message);

    /**
     * @brief Generates a random float between min_percent and max_percent.
     * @param min_percent Minimum bound of random percentage (e.g., 0.90).
     * @param max_percent Maximum bound of random percentage (e.g., 1.10).
     * @return Random float in range [min_percent, max_percent].
     */
    float random_percent(float min_percent, float max_percent);
};
