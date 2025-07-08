# POOSE3 ProjectGamma

## Idea of the Mercatoria marketplace

Welcome to Mercatoria—a bustling, medieval-inspired city where trade reigns supreme. In this dynamic marketplace, players take on the role of influential guild leaders, each representing a distinct trade such as blacksmithing, cloth making, spice dealing, or alchemy.

Core functionality:
1. Resource Management and Trade 
Players must acquire and sell goods.

Supply and demand shape a dynamic economy—prices rise and fall accordingly.

2. Marketplace & Bidding System
Market stalls and trade rights are limited and auctioned off regularly.

Players can bid publicly or negotiate secretly (in multiplayer).

Strategic placement and timing affect profits.

3. Random Events & Event Cards (optional)
Plagues, fires, trade booms, royal decrees, festivals, and wars affect the marketplace.

Events create scarcity or surplus, affecting prices and opening opportunities.

## Documentation
The Full Documentation of our CPP Backend is found in /Wiki. There you can find a UML Diagramm and Doxygenfiles that are used to comment. Also you can look inside the Code to check up on detailed functionality.

### Function Documentation of CPP 

### Constructor & Destructor

- **`Server()`**  
  Initializes the server, loads products, and starts the background thread to monitor session inactivity and automatically log out idle users.

- **`~Server()`**  
  Stops the inactivity monitoring thread cleanly on server shutdown.

---

### User Management

- **`bool register_user(const std::string &username, const std::string &password)`**  
  Registers a new user with the given username and password.  
  Throws if username or password is empty. Returns false if username already exists.

- **`int signin(const std::string &username, const std::string &password)`**  
  Signs in a user with credentials.  
  Returns a new session ID if successful, or -1 if login fails. Throws if user is already logged in or parameters are invalid.

- **`void signout(int session_id)`**  
  Signs out the user associated with the session ID.  
  Throws if session ID is invalid or not active.

- **`User &getuserbysessionid(int session_id)`**  
  Retrieves the User object for the given session ID.  
  Throws if session ID is invalid or expired.

- **`bool checkifsessionidisvalid(int session_id)`**  
  Checks if the session ID corresponds to an active session.

---

### Inventory and Wallet

- **`std::map<int, int> list_of_own_inventory(int session_id)`**  
  Returns the product inventory (product IDs and quantities) owned by the user associated with the session.

- **`float wallet(int session_id)`**  
  Returns the current wallet balance of the user.

---

### Product Transactions

- **`bool buy(int product_id, int session_id)`**  
  Attempts to buy one unit of the product for the user.  
  Returns true if successful; throws if the product is invalid or funds are insufficient.

- **`bool sell(int product_id, int session_id)`**  
  Attempts to sell one unit of the product from the user’s inventory.  
  Returns true if successful; throws if product not owned or invalid.

- **`std::list<Product> products_list(int session_id)`**  
  Returns a list of all products available for purchase/sale.

- **`float product_price(int product_id, int session_id)`**  
  Returns the current price of the specified product.

---

### Chat System

- **`std::list<std::string> getmessages(int session_id)`**  
  Retrieves all chat messages for the user.

- **`void sendmessage(int session_id, const std::string &message)`**  
  Sends a chat message from the user.

---

### Utility

- **`float random_percent(float min_percent, float max_percent)`**  
  Generates a random float percentage between the specified bounds, used internally for price fluctuations.

---

### Notes

- Most functions throw `std::runtime_error` when passed invalid session IDs or if operations fail.
- Session-related operations automatically update the last activity timestamp to prevent premature logout.
- The class is designed to be thread-safe where needed, especially around session management.


## Unittests of CPP Backend 

### Overview

The tests use the [Google Test](https://github.com/google/googletest) framework and cover the following areas:

- **User Registration & Authentication**  
  Tests for registering users, preventing duplicates, signing in and out, session management, and handling invalid credentials.

- **Product Buying and Selling**  
  Validation of buying and selling products, inventory updates, and error handling when operations exceed limits or use invalid sessions.

- **Wallet Balance Management**  
  Ensures wallet balance updates correctly after buy and sell operations.

- **Session and Product Validation**  
  Tests to ensure invalid session IDs or product IDs cause appropriate exceptions.

- **Product Price and Timestamp Updates**  
  Confirms that product prices and price history timestamps change after transactions.

### Test Fixture

- `ServerUsersTest` sets up a clean `Server` instance for each test.
- Provides helper methods to register users, sign in, and access products.

### Running the Tests

Make sure you have Google Test installed and properly configured in your build system.

Build and run tests using:

make
./unittests


## Demofiles 
This project includes two demo programs showcasing how to interact with the `Server` class for user registration, product buying/selling, and tracking product price history and wallet balances.

---

### Demo 1: Single User Product Purchase and Price History

- Registers and signs in a single user.
- Retrieves the product list and selects product with ID 1.
- Performs multiple purchase operations on product 1 to trigger price updates.
- Fetches updated product data and prints the full price history after the purchases.

**Purpose:**  
Demonstrates how buying products affects their price history and illustrates the need to refresh product data after transactions.

---

### Demo 2: Multi-User Buy and Sell Interaction with Inventory and Wallet Tracking

- Registers and signs in multiple users.
- Displays the initial product list.
- Simulates multiple rounds where each user attempts to buy and sell products in a cyclical manner.
- Shows user inventories after each buy/sell operation.
- Catches and reports errors like insufficient balance or invalid actions.
- Prints final wallet balances of all users.
- Displays price history of all products after the interactions.

**Purpose:**  
Simulates concurrent multi-user activity, demonstrating inventory and wallet management alongside dynamic product price changes.

---

### How to Run

Compile (cmake CMakeLists.txt make ) and run these demos after building your project. ./demo1 or ./demo2 you can run these Demo files.

---

Our demo Files for the cpp part, server, and client are found in /demo

## Server

We use python FastAPI as API Server with PyBind11 to combine it with the CPP implementation. Documentation is accessible after running the server with `http://localhost:8000/docs`.

There are multiple ways to start the server, the recommended way is via docker.
If you want to run the API on your own Laptop you can clone the repository and create a new Container with the Dockerfile

```
docker build --tag mercatori .
docker run -p 8000:8000 mercatori
```

## Client

The client needs a running server, to be started.
After starting the server, install the python dependencies with `uv sync` or Nix Flake (`nix develop`).
Then you can start the server inside the `client` directory with `python3 login_window.py`.

## Implementation Details

### CPP (Pascal)
We use CPP to implement the Game Logic and Player Management. 

### Pybind (Pascal)
We use pybind to make the cpp functions available in python 

### Server (Leona)
We Use Python to implement our Server Sockets needed. 

### Client (Olsi)
We use Python and PyQt for a Client GUI.

### Chat (Leona + Olsi)
We use Websockets between the API server and the client.

## Authors 
Pascal Haag, Leona Maroni, Olsi Manco
