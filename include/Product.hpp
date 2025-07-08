#pragma once
#include <string>
#include <map>

class Product {
private:
    int product_id;
    std::string product_name;
    float current_price;
    std::map<int64_t, float> price_history; // timestamp -> price

public:
    Product(int product_id, std::string product_name, float start_price); //constructor

    // Getters
    /**
     * @brief Returns the unique identifier for the product.
     * 
     * @return int 
     */
    int getproductid() const;
    /**
     * @brief Returns the name of the product.
     * 
     * @return std::string 
     */
    std::string getproductname() const;
    /**
     * @brief Returns the current price of the product.
     * 
     * @return float 
     */
    float getcurrentprice() const;
    /**
     * @brief Returns the price history of the product, mapping timestamps to prices.
     * @note timestamps are microsenconds since epoch 
     * @note timestamps -> price 
     * @return std::map<int64_t, float> 
     */
    std::map<int64_t, float> getpricehistory() const;

    // Setters
    /**
     * @brief Sets the unique identifier for the product.
     * 
     * @param ID 
     */
    void setproductid(int ID);
    /**
     * @brief Sets the name of the product.
     * 
     * @param name 
     */
    void setproductname(const std::string& name);
    /**
     * @brief Sets the current price of the product and updates the price history with the current timestamp.
     * 
     * @param price 
     */
    void setcurrentprice(float price);
};

