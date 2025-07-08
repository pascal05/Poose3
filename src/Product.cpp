#include "Product.hpp"
#include <chrono>
#include <iostream>
// Constructor definition
Product::Product(int id, std::string name, float start_price)
{
    product_id = id;
    product_name = name;
    Product::setcurrentprice(start_price); // Initialize current price and price history    
}
// Getter for product ID
int Product::getproductid() const
{
    return product_id;
}

// Setter for product ID
void Product::setproductid(int ID)
{
    product_id = ID;
}

// Getter for product name
std::string Product::getproductname() const
{
    return product_name;
}

// Setter for product name
void Product::setproductname(const std::string &name)
{
    product_name = name;
}

// Getter for current price
float Product::getcurrentprice() const
{
    return current_price;
}
void Product::setcurrentprice(float price)
{

    // Get current time in microseconds since epoch as int64_t
    auto now = std::chrono::system_clock::now();
    auto micros = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
    int64_t timestamp = static_cast<int64_t>(micros);

    // Update price history with the new price at the current timestamp
    price_history[timestamp] = price;
    current_price = price;
}



// Getter for price history
std::map<int64_t, float> Product::getpricehistory() const
{
    return price_history;
}
