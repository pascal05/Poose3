#include "User.hpp"

// Constructor implementation
User::User(const int userid, const std::string &username, const std::string &password)
    : userid(userid),
      username(username),
      password(password),
      aktive_session_id(-1), // default: not logged in
      wallet(100.0f)         // default starting money (adjust as needed)
{
    // stock starts empty
}

// Getter for username
std::string User::getusername() const
{
    return username;
}

// Setter for username
void User::setusername(const std::string &name)
{
    username = name;
}

// Getter for password
std::string User::getpassword() const
{
    return password;
}

// Setter for password
void User::setpassword(const std::string &pwd)
{
    password = pwd;
}

// Getter for aktive_session_id
int User::getaktivesessionid() const
{
    return aktive_session_id;
}

// Setter for aktive_session_id
void User::setaktivesessionid(int session_id)
{
    aktive_session_id = session_id;
}

// Getter for wallet
float User::getwallet() const
{
    return wallet;
}

// Setter for wallet
void User::setwallet(float amount)
{
    wallet = amount;
}

// Getter for stock
std::map<int, int> User::getstock() const
{
    return stock;
}

// Setter for stock
void User::setstock(const std::map<int, int> &newstock)
{
    stock = newstock;
}

int User::getuserID() const
{
    return userid;
}
void User::setuserID(int id)
{
    userid = id;
}