#include "general_includes.h"
#include "helpers.h"
#include "path.h"
#include "requests.h"

int main(int argc, char *argv[]) {
    int sockfd;
    string message, response;
    const char *info = "/catalog/products/1";

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "Register with GET before logging" << endl;
    message = compute_get_request("localhost", REGISTER, GET, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "LOGIN with GET " << endl;
    message = compute_get_request("localhost", LOGIN, GET, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "LOGOUT without being logged in " << endl;
    message = compute_get_request("localhost", LOGOUT, GET, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "GET products without being logged in " << endl;
    message = compute_get_request("localhost", CATALOG, GET, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "GET info about a product without being logged in" << endl;
    message = compute_get_request("localhost", info, GET, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "REGISTER without credentials" << endl;
    message = compute_post_request("localhost", REGISTER, CONTENT_TYPE_JSON,
                                   "", "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    json log;
    log["username"] = "adrian";
    log["password"] = "password";

    json wrong_log;
    wrong_log["username"] = "adrian";
    wrong_log["password"] = "passwor";

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "REGISTER with credentials" << endl;
    message = compute_post_request("localhost", REGISTER, CONTENT_TYPE_JSON,
                                   log, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "GET all products without being logged in" << endl;
    message = compute_get_request("localhost", CATALOG, GET, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    // sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    // cout << "GET all products with a cookie" << endl;
    // message = compute_get_request("localhost", CATALOG, GET, "cookie", "");
    // cout << message << endl;
    // send_message(sockfd, message.c_str());
    // response = receive(sockfd);
    // cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "Login with wrong credentials" << endl;
    message = compute_post_request("localhost", LOGIN, CONTENT_TYPE_JSON,
                                   wrong_log, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "Login with credentials" << endl;
    message = compute_post_request("localhost", LOGIN, CONTENT_TYPE_JSON,
                                   log, "", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "GET all products" << endl;
    message = compute_get_request("localhost", CATALOG, GET, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    json prod;
    prod["name"] = "apple";
    prod["category"] = "fruits";
    prod["price"] = 15;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "Add a new product" << endl;
    message = compute_post_request("localhost", CATALOG, CONTENT_TYPE_JSON,
                                   prod, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    prod["name"] = "Honda";
    prod["category"] = "cars";
    prod["price"] = 15986;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "Add a new product" << endl;
    message = compute_post_request("localhost", CATALOG, CONTENT_TYPE_JSON,
                                   prod, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "GET all products" << endl;
    message = compute_get_request("localhost", CATALOG, GET, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "GET only ONE product" << endl;
    message = compute_get_request("localhost", info, GET, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "DELETE first product" << endl;
    message = compute_get_request("localhost", info, DELETE, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "GET all products" << endl;
    message = compute_get_request("localhost", CATALOG, GET, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "a normal LOGOUT" << endl;
    message = compute_get_request("localhost", LOGOUT, GET, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    sockfd = open_connection("127.0.0.1", 12345, AF_INET, SOCK_STREAM, 0);
    cout << "LOGOUT if is already logged out" << endl;
    message = compute_get_request("localhost", LOGOUT, GET, "cookie", "");
    cout << message << endl;
    send_message(sockfd, message.c_str());
    response = receive(sockfd);
    cout << response << endl;

    return 0;
}