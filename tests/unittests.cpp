#include <gtest/gtest.h>
#include "Server.hpp"

// Test fixture for fresh Server instance and helpers
class ServerUsersTest : public ::testing::Test {
protected:
    Server server;

    int register_and_signin(const std::string& username, const std::string& password) {
        EXPECT_TRUE(server.register_user(username, password));
        int session_id = server.signin(username, password);
        EXPECT_GT(session_id, 0);
        return session_id;
    }

    // Get first product copy (not pointer!) from products_list()
    Product get_first_product(int session_id) {
        auto products = server.products_list(session_id);
        EXPECT_FALSE(products.empty());
        return products.front();
    }
};

// --- User Registration & Session Tests ---

TEST_F(ServerUsersTest, RegisterAndLoginReturnsSessionId) {
    int session_id = register_and_signin("user1", "pass1");
    EXPECT_GT(session_id, 0);
    server.signout(session_id);
}

TEST_F(ServerUsersTest, RegisterDuplicateUserFails) {
    EXPECT_TRUE(server.register_user("user1", "pass1"));
    EXPECT_FALSE(server.register_user("user1", "pass1"));
}

TEST_F(ServerUsersTest, EmptyUsernameOrPasswordThrows) {
    EXPECT_THROW(server.register_user("", "pass"), std::runtime_error);
    EXPECT_THROW(server.register_user("user", ""), std::runtime_error);
}

TEST_F(ServerUsersTest, WrongPasswordReturnsMinusOne) {
    server.register_user("user1", "pass1");
    int session_id = server.signin("user1", "wrongpass");
    EXPECT_EQ(session_id, -1);
}

TEST_F(ServerUsersTest, DoubleSigninThrows) {
    server.register_user("user1", "pass1");
    int session_id = server.signin("user1", "pass1");
    EXPECT_THROW(server.signin("user1", "pass1"), std::runtime_error);
    server.signout(session_id);
}

TEST_F(ServerUsersTest, SignOutThenSignInNewSessionId) {
    server.register_user("user1", "pass1");
    int session1 = server.signin("user1", "pass1");
    server.signout(session1);
    int session2 = server.signin("user1", "pass1");
    EXPECT_NE(session1, session2);
    server.signout(session2);
}

TEST_F(ServerUsersTest, SignOutInvalidSessionThrows) {
    EXPECT_THROW(server.signout(9999), std::runtime_error);
}

TEST_F(ServerUsersTest, DifferentUsersGetUniqueSessionIds) {
    server.register_user("user1", "pass1");
    server.register_user("user2", "pass2");
    int session1 = server.signin("user1", "pass1");
    int session2 = server.signin("user2", "pass2");
    EXPECT_NE(session1, session2);
    server.signout(session1);
    server.signout(session2);
}

// --- Buy / Sell Product Tests ---

TEST_F(ServerUsersTest, BuyProductSuccess) {
    int session_id = register_and_signin("buyer", "password");
    Product product = get_first_product(session_id);
    int product_id = product.getproductid();

    for (int i = 0; i < 3; ++i) {
        EXPECT_TRUE(server.buy(product_id, session_id));
    }

    auto inventory = server.list_of_own_inventory(session_id);
    EXPECT_EQ(inventory[product_id], 3);

    server.signout(session_id);
}

TEST_F(ServerUsersTest, BuyProductInsufficientFails) {
    int session_id = register_and_signin("buyer", "password");
    Product product = get_first_product(session_id);
    int product_id = product.getproductid();

    int buy_count = 0;
    try {
        while (true) {
            if (!server.buy(product_id, session_id)) break;
            buy_count++;
            if (buy_count > 100) break;  // fail-safe
        }
    } catch (const std::runtime_error& e) {
        EXPECT_STREQ(e.what(), "Insufficient wallet balance");
    }
    EXPECT_GT(buy_count, 0);

    EXPECT_THROW(server.buy(product_id, session_id), std::runtime_error);

    server.signout(session_id);
}

TEST_F(ServerUsersTest, SellProductSuccess) {
    int session_id = register_and_signin("buyer", "password");
    Product product = get_first_product(session_id);
    int product_id = product.getproductid();

    for (int i = 0; i < 5; ++i) {
        EXPECT_TRUE(server.buy(product_id, session_id));
    }

    for (int i = 0; i < 2; ++i) {
        EXPECT_TRUE(server.sell(product_id, session_id));
    }

    auto inventory = server.list_of_own_inventory(session_id);
    EXPECT_EQ(inventory[product_id], 3);

    server.signout(session_id);
}

TEST_F(ServerUsersTest, SellMoreThanOwnedThrows) {
    int session_id = register_and_signin("buyer", "password");
    Product product = get_first_product(session_id);
    int product_id = product.getproductid();

    EXPECT_TRUE(server.buy(product_id, session_id));
    EXPECT_TRUE(server.buy(product_id, session_id));

    EXPECT_TRUE(server.sell(product_id, session_id));
    EXPECT_TRUE(server.sell(product_id, session_id));

    EXPECT_THROW(server.sell(product_id, session_id), std::runtime_error);

    server.signout(session_id);
}

TEST_F(ServerUsersTest, BuyInvalidSessionThrows) {
    int valid_session = register_and_signin("buyer", "password");
    Product product = get_first_product(valid_session);
    int product_id = product.getproductid();

    EXPECT_THROW(server.buy(product_id, 99999), std::runtime_error);

    server.signout(valid_session);
}

TEST_F(ServerUsersTest, SellInvalidSessionThrows) {
    int valid_session = register_and_signin("buyer", "password");
    Product product = get_first_product(valid_session);
    int product_id = product.getproductid();

    EXPECT_THROW(server.sell(product_id, 99999), std::runtime_error);

    server.signout(valid_session);
}

// --- Wallet Balance Change Tests ---

TEST_F(ServerUsersTest, WalletBalanceDecreasesAfterBuy) {
    int session_id = register_and_signin("wallet_user", "pw");
    Product product = get_first_product(session_id);

    float before = server.wallet(session_id);
    EXPECT_TRUE(server.buy(product.getproductid(), session_id));
    float after = server.wallet(session_id);

    EXPECT_LT(after, before);
    server.signout(session_id);
}

TEST_F(ServerUsersTest, WalletBalanceIncreasesAfterSell) {
    int session_id = register_and_signin("wallet_user2", "pw");
    Product product = get_first_product(session_id);
    int product_id = product.getproductid();

    server.buy(product_id, session_id);
    float before = server.wallet(session_id);
    server.sell(product_id, session_id);
    float after = server.wallet(session_id);

    EXPECT_GT(after, before);
    server.signout(session_id);
}

TEST_F(ServerUsersTest, SellZeroQuantityFails) {
    int session_id = register_and_signin("zero_sell", "pw");
    Product product = get_first_product(session_id);
    int product_id = product.getproductid();

    EXPECT_THROW(server.sell(product_id, session_id), std::runtime_error);
    server.signout(session_id);
}

// --- Invalid session or product tests ---

TEST_F(ServerUsersTest, SessionIdZeroFails) {
    int session_id = 0;
    EXPECT_THROW(server.products_list(session_id), std::runtime_error);
    EXPECT_THROW(server.signout(session_id), std::runtime_error);
}

TEST_F(ServerUsersTest, InvalidProductIdThrows) {
    int session_id = register_and_signin("invalid_product", "pw");
    int invalid_product_id = -1;

    EXPECT_THROW(server.buy(invalid_product_id, session_id), std::runtime_error);
    EXPECT_THROW(server.sell(invalid_product_id, session_id), std::runtime_error);

    server.signout(session_id);
}

// --- Price and Timestamp Change Tests ---

auto get_price_and_timestamp = [](const Product& p) -> std::pair<float, int64_t> {
    float price = p.getcurrentprice();
    const auto& history = p.getpricehistory();
    int64_t latest_ts = 0;
    if (!history.empty()) {
        latest_ts = history.rbegin()->first;
    }
    return {price, latest_ts};
};

TEST_F(ServerUsersTest, BuyProductPriceAndTimestampChange) {
    int session_id = register_and_signin("buyer", "password");
    auto products = server.products_list(session_id);
    ASSERT_FALSE(products.empty());

    int product_id = products.front().getproductid();

    // Find product copy before buy
    const Product* product_before_ptr = nullptr;
    for (const auto& p : products) {
        if (p.getproductid() == product_id) {
            product_before_ptr = &p;
            break;
        }
    }
    ASSERT_NE(product_before_ptr, nullptr);
    const Product& product_before = *product_before_ptr;

    auto [price_before, ts_before] = get_price_and_timestamp(product_before);

    EXPECT_TRUE(server.buy(product_id, session_id));

    auto products_after = server.products_list(session_id);

    // Find product copy after buy
    const Product* product_after_ptr = nullptr;
    for (const auto& p : products_after) {
        if (p.getproductid() == product_id) {
            product_after_ptr = &p;
            break;
        }
    }
    ASSERT_NE(product_after_ptr, nullptr);
    const Product& product_after = *product_after_ptr;

    auto [price_after, ts_after] = get_price_and_timestamp(product_after);

    EXPECT_NE(price_before, price_after);
    EXPECT_GE(ts_after, ts_before);

    server.signout(session_id);
}

TEST_F(ServerUsersTest, SellProductPriceAndTimestampChange) {
    int session_id = register_and_signin("buyer", "password");
    auto products = server.products_list(session_id);
    ASSERT_FALSE(products.empty());

    int product_id = products.front().getproductid();

    // Find product copy before buy
    const Product* product_before_ptr = nullptr;
    for (const auto& p : products) {
        if (p.getproductid() == product_id) {
            product_before_ptr = &p;
            break;
        }
    }
    ASSERT_NE(product_before_ptr, nullptr);
    const Product& product_before = *product_before_ptr;

    EXPECT_TRUE(server.buy(product_id, session_id));

    auto [price_before, ts_before] = get_price_and_timestamp(product_before);

    EXPECT_TRUE(server.sell(product_id, session_id));

    auto products_after = server.products_list(session_id);

    // Find product copy after sell
    const Product* product_after_ptr = nullptr;
    for (const auto& p : products_after) {
        if (p.getproductid() == product_id) {
            product_after_ptr = &p;
            break;
        }
    }
    ASSERT_NE(product_after_ptr, nullptr);
    const Product& product_after = *product_after_ptr;

    auto [price_after, ts_after] = get_price_and_timestamp(product_after);

    EXPECT_NE(price_before, price_after);
    EXPECT_GE(ts_after, ts_before);

    server.signout(session_id);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
