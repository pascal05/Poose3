#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include "Server.hpp"
#include "Product.hpp"

// Ensure the following lines are present in Server.hpp and Product.hpp:
// class Product { public: int getproductid(); std::string getproductname(); float getcurrentprice(); std::vector<float> getpricehistory(); void setproductid(int); void setproductname(const std::string&); void setcurrentprice(float); };
// class Server { public: Server(); bool signin(const std::string&, const std::string&); bool register_user(const std::string&, const std::string&); void signout(const std::string&); bool checkifsessionidisvalid(const std::string&); std::vector<Product> list_of_own_inventory(const std::string&); float wallet(const std::string&); bool buy(int, const std::string&); bool sell(int, const std::string&); std::vector<Product> products_list(const std::string&); float product_price(int, const std::string&); std::vector<std::string> getmessages(const std::string&); void sendmessage(const std::string&, const std::string&); };

namespace py = pybind11;

PYBIND11_MODULE(server_module, m) {
    // Bind Product class (by value, no shared_ptr needed)
    py::class_<Product>(m, "Product")
        .def(py::init<int, std::string, float>(), py::arg("product_id"), py::arg("product_name"), py::arg("start_price"))
        // getters
        .def("getproductid", &Product::getproductid)
        .def("getproductname", &Product::getproductname)
        .def("getcurrentprice", &Product::getcurrentprice)
        .def("getpricehistory", &Product::getpricehistory)
        // setters
        .def("setproductid", &Product::setproductid, py::arg("ID"))
        .def("setproductname", &Product::setproductname, py::arg("name"))
        .def("setcurrentprice", &Product::setcurrentprice, py::arg("price"))
        ;

    // Bind Server class with shared_ptr for safer lifetime management
    py::class_<Server, std::shared_ptr<Server>>(m, "Server")
        .def(py::init<>())
        .def("signin", &Server::signin, py::arg("username"), py::arg("password"))
        .def("register_user", &Server::register_user, py::arg("username"), py::arg("password"))
        .def("signout", &Server::signout, py::arg("session_id"))
        .def("checkifsessionidisvalid", &Server::checkifsessionidisvalid, py::arg("session_id"))
        .def("list_of_own_inventory", &Server::list_of_own_inventory, py::arg("session_id"))
        .def("wallet", &Server::wallet, py::arg("session_id"))
        .def("buy", &Server::buy, py::arg("product_id"), py::arg("session_id"))
        .def("sell", &Server::sell, py::arg("product_id"), py::arg("session_id"))
        .def("products_list", &Server::products_list, py::arg("session_id"))
        .def("product_price", &Server::product_price, py::arg("product_id"), py::arg("session_id"))
        .def("getmessages", &Server::getmessages, py::arg("session_id"))
        .def("sendmessage", &Server::sendmessage, py::arg("session_id"), py::arg("message"))
        ;
}
