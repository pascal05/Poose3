#pragma once 
#include <string>
#include <map>

class User {
private:
    int userid; // Unique identifier for the user
    std::string username;
    std::string password;
    int aktive_session_id;
    float wallet;
    std::map<int, int> stock; // product_id -> number owned

public:
    /**
     * @brief Construct a new User object with userid, username, and password
     * 
     * @param userid 
     * @param username 
     * @param password 
     */
    User(const int userid, const std::string& username, const std::string& password);
    // Getters
    /**
     * @brief Returns the username of the user.
     * 
     * @return std::string 
     */
    std::string getusername() const;
    /**
     * @brief Returns the password of the user.
     * 
     * @return std::string 
     */
    std::string getpassword() const;
    /**
     * @brief Returns the active session ID of the user.
     * 
     * @return int 
     */
    int getaktivesessionid() const;
    /**
     * @brief Returns the wallet balance of the user.
     * 
     * @return float 
     */
    float getwallet() const;
    /**
     * @brief Returns the stock of the user, where the key is product_id and the value is the quantity owned.
     * 
     * @return std::map<int, int> 
     */
    std::map<int, int> getstock() const;
    /**
     * @brief Returns the user ID.
     * 
     * @return int 
     */
    int getuserID() const;
    // Setters
    /**
     * @brief Sets the username of the user.
     * 
     * @param username 
     */
    void setusername(const std::string& username);
    /**
     * @brief Sets the password of the user.
     * 
     * @param password 
     */
    void setpassword(const std::string& password);
    /**
     * @brief Sets the active session ID of the user.
     * 
     * @param session_id 
     */
    void setaktivesessionid(int session_id);
    /**
     * @brief Sets the wallet balance of the user.
     * 
     * @param wallet 
     */
    void setwallet(float wallet);
    /**
     * @brief Sets the stock of the user, where the key is product_id and the value is the quantity owned.
     * 
     * @param stock 
     */
    void setstock(const std::map<int, int>& stock);
    /**
     * @brief Sets the user ID.
     * 
     * @param userid 
     */
    void setuserID(int userid); 
};