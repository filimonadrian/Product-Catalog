#include "helpers.h"
#include "path.h"
#include "response.h"
#include "server_helper.h"

void usage(char *file) {
    fprintf(stderr, "Usage: %s server_port\n", file);
    exit(0);
}

void close_all_sock(fd_set *fd, int fdmax) {
    for (int sock = 0; sock < fdmax; sock++) {
        if (FD_ISSET(sock, fd)) {
            close(sock);
        }
    }
}

bool close_server(fd_set read_fds, int fdmax) {
    char exit_message[10];
    memset(exit_message, 0, 10);
    fgets(exit_message, 6, stdin);
    if (!strcmp(exit_message, "quit\n")) {
        close_all_sock(&read_fds, fdmax);
        printf("Exiting..\n");
        return true;
    }
    printf("That is not a valid command -_- \n");
    return false;
}

// split the sentence in words
vector<string> tokenize(string comm) {
    vector<string> results;
    // boost::split(results, comm, [](char c) { return c == ' '; });
    boost::split(results, comm, boost::is_any_of(" "));
    return results;
}
vector<string> tokenize(string comm, const char escape[]) {
    vector<string> results;
    boost::split(results, comm, boost::is_any_of(escape));

    return results;
}

char *not_found(const char *message) {
    char err[] = "HTTP/1.1 404 Not Found\r\n";
    return strcat(err, message);
}

void send_error(int sockfd) {
    char s[BUFLEN];
    memset(s, 0, BUFLEN);
    strcpy(s, "HTTP/1.1 404 Not Found\r\nIncorrect parsing data\r\n");
    int ret = send(sockfd, s, strlen(s), 0);
    if (ret < 0) {
        perror("SEND ERROR");
    }
}
// returns number or -1
int check_number(string data) {
    int number = -1;
    stringstream parse(data);
    parse >> number;

    return number;
}
/* returns 0 if the data is correct */
int parse_json_credentials(string data, string &username, string &password) {
    vector<string> credentials;
    char data_s[BUFLEN];
    memset(data_s, 0, BUFLEN);
    credentials = tokenize(data, ",");

    int i = strlen("{\"username\":\"");
    strcpy(data_s, credentials[0].c_str());

    if (strncmp(data_s, "{\"username\":\"", i)) {
        return 1;
    }

    while (1) {
        if (data_s[i] == '"') {
            break;
        }
        username.push_back(data_s[i]);
        i++;
    }

    i = strlen("\"password\":\"");
    memset(data_s, 0, BUFLEN);
    strcpy(data_s, credentials[1].c_str());

    if (strncmp(data_s, "\"password\":\"", i)) {
        return 1;
    }

    while (1) {
        if (data_s[i] == '"') {
            break;
        }
        password.push_back(data_s[i]);
        i++;
    }

    return 0;
}
/* receives the data and returns a product */
int parse_json_product(string data, product &p) {
    vector<string> prod;
    char data_s[BUFLEN];
    memset(data_s, 0, BUFLEN);
    string aux;

    prod = tokenize(data, ",");

    memset(data_s, 0, BUFLEN);
    aux.clear();
    int i = strlen("{\"name\":\"");
    strcpy(data_s, prod[0].c_str());
    if (strncmp(data_s, "{\"name\":\"", i)) {
        // send_error(sockfd);
        return 1;
    }

    while (1) {
        if (data_s[i] == '"') {
            break;
        }
        aux.push_back(data_s[i]);
        i++;
    }
    p.name = aux;

    memset(data_s, 0, BUFLEN);
    aux.clear();
    i = strlen("\"category\":\"");
    strcpy(data_s, prod[1].c_str());
    if (strncmp(data_s, "\"category\":\"", i)) {
        return 1;
    }

    while (1) {
        if (data_s[i] == '"') {
            break;
        }
        aux.push_back(data_s[i]);
        i++;
    }

    p.category = aux;

    memset(data_s, 0, BUFLEN);
    aux.clear();
    i = strlen("\"price\":");
    strcpy(data_s, prod[2].c_str());

    if (strncmp(data_s, "\"price\":", i)) {
        return 1;
    }

    while (1) {
        if (data_s[i] == '}') {
            break;
        }
        aux.push_back(data_s[i]);
        i++;
    }

    int price = check_number(aux);
    if (price < 0) {
        return 1;
    }

    p.price = price;
    p.id = product_id;
    product_id++;

    time_t my_time;
    time(&my_time);
    p.created_date = ctime(&my_time);
    p.updated_date = ctime(&my_time);
    p.created_date.pop_back();
    p.updated_date.pop_back();

    return 0;
}

void print_products(unordered_map<int, product> catalog) {
    cout << "ALL PRODUCTS : \n";
    product el;
    for (auto entry : catalog) {
        el = entry.second;
        cout << entry.first << " ";
        cout << el.id << " "
             << el.name << " "
             << el.category << " "
             << el.created_date << " "
             << el.updated_date
             << endl;

        cout << endl;
    }
}

vector<string> do_segment(char *sentence) {
    vector<string> lines;
    std::stringstream ss(sentence);
    std::string to;

    if (sentence != NULL) {
        while (std::getline(ss, to, '\n')) {
            lines.push_back(to);
        }
    }

    return lines;
}

int check_HTTP_version(string to_test) {
    if (!to_test.compare(HTTP_VERSION0) || !to_test.compare(HTTP_VERSION1)) {
        return 0;
    }
    return 1;
}

int check_host(string to_test) {
    if (!to_test.compare(HOSTNAME)) {
        return 0;
    }
    return 1;
}

int get_id(vector<string> first_line) {
    vector<string> frag_path;
    int id = -1;

    auto path = first_line[1];
    frag_path = tokenize(path, "/");

    /* id is the last element from the path */
    /* check if the id is integer(> 0) */
    stringstream parse(frag_path[frag_path.size() - 1]);
    parse >> id;

    //  if (!strncmp(PRODUCT_INFO, path.c_str(), strlen(PRODUCT_INFO)) {}
    if (id < 0) {
        // throw an error -> the product doesn't exist
        return -1;
    }
    return id;
}

int check_cookie(vector<string> lines, string &cookie) {
    vector<string> cookie_line;
    for (unsigned long i = 1; i < lines.size(); i++) {
        if (!lines[i].compare(0, 7, "Cookie:")) {
            if ((lines[i])[7] == ' ') {
                cookie_line = tokenize(lines[i]);
            } else {
                cookie_line = tokenize(lines[i], ":");
            }
            cookie = cookie_line[1];
            return 0;
        }
    }

    return 1;
}

int check_is_on(unordered_map<string, user> cookies, string cookie) {
    if (cookies.find(cookie) != cookies.end()) {
        return 0;
    }
    return 1;
}

string wrap_product(product p) {
    json prod;
    prod["id"] = p.id;
    prod["price"] = p.price;
    prod["name"] = p.name;
    prod["category"] = p.category;
    prod["created_date"] = p.created_date;
    prod["updated_date"] = p.updated_date;

    return prod.dump();
}

string wrap_all_products(unordered_map<int, product> catalog) {
    string s_catalog;
    s_catalog += "[";
    for (auto prod : catalog) {
        s_catalog += wrap_product(prod.second);
        s_catalog += ",";
    }
    s_catalog.pop_back();
    s_catalog += "]";
    return s_catalog;
}

string wrap_cookie(string cookie) {
    string parse;

    parse += "{\"";
    parse += cookie;
    parse += "\"}";

    return parse;
}
/* compute all get requests */
/* getting acces, all products, product with id, logout */
void get_request(vector<string> lines, vector<string> first_line, int sockfd,
                 unordered_map<int, product> &catalog,
                 unordered_map<string, user> &cookies,
                 unordered_map<string, user> &users) {
    string aux;
    string cookie, data, response;

    if (!first_line[1].compare(ACCESS)) {
        /* generate a JWT TOKEN */

    } else if (!first_line[1].compare(LOGOUT)) {
        /* if the user provides the cookie and is on */
        if (!check_cookie(lines, cookie) && !check_is_on(cookies, cookie)) {
            if (!logout(users, cookies, cookie)) {
                response = compute_response(OK, HOSTNAME, CONTENT_TYPE_TEXT,
                                            "", "",
                                            "Successfully logged out\n");
                send_message(sockfd, response.c_str());
            } else {
                response = compute_response(FORBIDDEN, HOSTNAME,
                                            CONTENT_TYPE_TEXT,
                                            "", "",
                                            "You're not logged in\n");
                send_message(sockfd, response.c_str());
            }
        } else {
            response = compute_response(UNAUTHORIZED, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\"You're not allowed to do this\"\n");
            send_message(sockfd, response.c_str());
        }
        /* send all the products to client */
    } else if (!first_line[1].compare(CATALOG)) {

        if (!check_cookie(lines, cookie) && !check_is_on(cookies, cookie)) {
            data = wrap_all_products(catalog);
            if (data.size() == 0) {

                response = compute_response(NO_CONTENT, HOSTNAME,
                                            CONTENT_TYPE_JSON,
                                            "", "",
                                            data);
            } else {
                response = compute_response(OK, HOSTNAME,
                                            CONTENT_TYPE_JSON,
                                            "", "",
                                            data);
            }
            send_message(sockfd, response.c_str());

        } else {
            response = compute_response(UNAUTHORIZED, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\"You're not allowed to do this\"\n");
            send_message(sockfd, response.c_str());
        }
        /* send just the product with that id */
    } else if (!first_line[1].compare(0, strlen(PRODUCT_INFO), PRODUCT_INFO)) {
        /* if the cookie is provided */
        if (!check_cookie(lines, cookie) && !check_is_on(cookies, cookie)) {

            int product_id = get_id(first_line);

            /* if id is a positive number and product with this id exists */
            if (product_id >= 0 && catalog.find(product_id) != catalog.end()) {
                data = wrap_product(catalog[product_id]);
                response = compute_response(OK, HOSTNAME,
                                            CONTENT_TYPE_JSON,
                                            "", "",
                                            data);
                send_message(sockfd, response.c_str());
            } else {
                response = compute_response(NOT_FOUND, HOSTNAME,
                                            CONTENT_TYPE_TEXT,
                                            "", "",
                                            "error:\"Product not Found\"\n");
                send_message(sockfd, response.c_str());
            }
        } else {
            response = compute_response(UNAUTHORIZED, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\"You're not allowed to do this\"\n");
            send_message(sockfd, response.c_str());
        }
    } else {
        response = compute_response(BAD_REQUEST, HOSTNAME,
                                    CONTENT_TYPE_TEXT,
                                    "", "",
                                    "error:\"Incorrect path\"\n");
        send_message(sockfd, response.c_str());
    }
}

// Post request --> Register, Login, Add
void post_request(vector<string> lines, vector<string> first_line, int sockfd,
                  unordered_map<int, product> &catalog,
                  unordered_map<string, user> &cookies,
                  unordered_map<string, user> &users) {

    string aux;
    string cookie, data, response;
    /* registration */
    if (!first_line[1].compare(REGISTER)) {

        if (!register_user(lines[lines.size() - 1], users)) {
            response = compute_response(CREATED, HOSTNAME, CONTENT_TYPE_TEXT,
                                        "", "",
                                        "User created!\n");
            send_message(sockfd, response.c_str());
        } else {
            response = compute_response(FORBIDDEN, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\"This username is taken\"\n");
            send_message(sockfd, response.c_str());
        }
        /* log in */
    } else if (!first_line[1].compare(LOGIN)) {
        string cookie;

        if (!login(lines[lines.size() - 1], users, cookies, cookie)) {
            response = compute_response(OK, HOSTNAME, CONTENT_TYPE_JSON,
                                        cookie, "",
                                        "");
            send_message(sockfd, response.c_str());
        } else {
            response = compute_response(FORBIDDEN, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\"Already logged in\"\n");
            send_message(sockfd, response.c_str());
        }

        /* add product */
    } else if (!first_line[1].compare(CATALOG)) {

        if (!check_cookie(lines, cookie) && !check_is_on(cookies, cookie)) {
            if (!add_product(lines[lines.size() - 1], catalog)) {
                response = compute_response(OK, HOSTNAME, CONTENT_TYPE_JSON,
                                            "", "",
                                            "");
                send_message(sockfd, response.c_str());
            } else {
                response = compute_response(BAD_REQUEST, HOSTNAME,
                                            CONTENT_TYPE_JSON,
                                            "", "",
                                            "error:\"Incorrect string parsing\"\n");
                send_message(sockfd, response.c_str());
            }
        } else {
            response = compute_response(UNAUTHORIZED, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\"You're not allowed to do this\"\n");
            send_message(sockfd, response.c_str());
        }

    } else {
        response = compute_response(BAD_REQUEST, HOSTNAME,
                                    CONTENT_TYPE_TEXT,
                                    "", "",
                                    "error:\"Incorrect path\"\n");
        send_message(sockfd, response.c_str());
    }
}

// Put request --> modify a product
void put_request(vector<string> lines, vector<string> first_line, int sockfd,
                 unordered_map<int, product> &catalog,
                 unordered_map<string, user> &cookies) {
    string cookie, data, response;
    /* modify this product */
    if (!first_line[1].compare(0, strlen(PRODUCT_INFO), PRODUCT_INFO)) {
        /* if the cookie is provided */
        if (!check_cookie(lines, cookie) && !check_is_on(cookies, cookie)) {

            int product_id = get_id(first_line);

            /* if id is a positive number and product with this id exists */
            if (product_id >= 0 && catalog.find(product_id) != catalog.end()) {
                if (!modify_product(lines[lines.size() - 1], catalog, product_id)) {
                    response = compute_response(OK, HOSTNAME,
                                                CONTENT_TYPE_JSON,
                                                "", "",
                                                data);
                    send_message(sockfd, response.c_str());
                } else {
                    response = compute_response(BAD_REQUEST, HOSTNAME,
                                                CONTENT_TYPE_TEXT,
                                                "", "",
                                                "error:\"Parsing error!\"\n");
                    send_message(sockfd, response.c_str());
                }
            } else {
                response = compute_response(NOT_FOUND, HOSTNAME,
                                            CONTENT_TYPE_TEXT,
                                            "", "",
                                            "error:\"Product not Found\"\n");
                send_message(sockfd, response.c_str());
            }
        } else {
            response = compute_response(UNAUTHORIZED, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\":You're not allowed to do this\"\n");
            send_message(sockfd, response.c_str());
        }
    }
}

void delete_request(vector<string> lines, vector<string> first_line, int sockfd,
                    unordered_map<int, product> &catalog,
                    unordered_map<string, user> &cookies) {
    string cookie, data, response;
    /* delete this product */
    if (!first_line[1].compare(0, strlen(PRODUCT_INFO), PRODUCT_INFO)) {
        /* if the cookie is provided */
        if (!check_cookie(lines, cookie) && !check_is_on(cookies, cookie)) {

            int product_id = get_id(first_line);

            /* if id is a positive number and product with this id exists */
            if (product_id >= 0 && catalog.find(product_id) != catalog.end()) {
                if (!delete_product(catalog, product_id)) {
                    response = compute_response(OK, HOSTNAME,
                                                CONTENT_TYPE_JSON,
                                                "", "",
                                                data);
                    send_message(sockfd, response.c_str());
                } else {
                    response = compute_response(BAD_REQUEST, HOSTNAME,
                                                CONTENT_TYPE_TEXT,
                                                "", "",
                                                "error:\"Parsing error!\"\n");
                    send_message(sockfd, response.c_str());
                }
            } else {
                response = compute_response(NOT_FOUND, HOSTNAME,
                                            CONTENT_TYPE_TEXT,
                                            "", "",
                                            "error:\"Product not Found\"\n");
                send_message(sockfd, response.c_str());
            }
        } else {
            response = compute_response(UNAUTHORIZED, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\":You're not allowed to do this\"\n");
            send_message(sockfd, response.c_str());
        }
    }
}

int register_user(string data, unordered_map<string, user> &users) {
    string username, password;
    user usr;

    parse_json_credentials(data, username, password);

    // user doesn't exist
    if (users.find(username) == users.end()) {
        usr.username = username;
        usr.password = password;
        users.insert({username, usr});
        return 0;
    }
    return 1;
}

int login(string data, unordered_map<string, user> &users,
          unordered_map<string, user> &cookies, string &cookie) {
    string username, password;
    parse_json_credentials(data, username, password);

    auto usr = users.find(username);
    // if user exists
    if (usr != users.end()) {
        cookie = "cookie";
        (*usr).second.cookie = cookie;
        cookies.insert({(*usr).second.cookie, (*usr).second});
        return 0;
    }
    return 1;
}

int logout(unordered_map<string, user> &users,
           unordered_map<string, user> &cookies,
           string cookie) {
    string username;

    auto entry = cookies.find(cookie);
    // if cookie exists
    if (entry != cookies.end()) {
        users[entry->second.username].cookie.clear();
        cookies.erase(cookie);
        return 0;
    }
    return 1;
}

// add product in catalog
int add_product(string data, unordered_map<int, product> &catalog) {
    product p;
    if (!parse_json_product(data, p)) {
        catalog.insert({p.id, p});
        return 0;
    }
    return 1;
}

int modify_product(string data, unordered_map<int, product> &catalog, int id) {
    product p;

    /* if the product with this id doesn't exist => ERROR */
    if (catalog.find(id) == catalog.end()) {
        return 1;
    }

    /* if the fields were provided correctly, update the product */
    if (!parse_json_product(data, p)) {
        catalog[id].category = p.category;
        catalog[id].price = p.price;
        catalog[id].name = p.name;
        time_t current_time;
        time(&current_time);
        catalog[id].updated_date = ctime(&current_time);
        catalog[id].updated_date.pop_back();

    } else {
        return 1;
    }

    return 0;
}

int delete_product(unordered_map<int, product> &catalog, int id) {

    /* if the product with this id doesn't exist => ERROR */
    if (catalog.find(id) == catalog.end()) {
        return 1;
    }

    catalog.erase(id);
    return 0;
}

int main(int argc, char *argv[]) {

    int sockfd, newsockfd, portno;
    char buffer[BUFLEN];
    struct sockaddr_in serv_addr, cli_addr;
    unordered_map<int, product> catalog;
    unordered_map<string, user> users;
    unordered_map<string, user> cookies;

    int ret;
    string s;
    socklen_t clilen;

    if (argc < 2) {
        usage(argv[0]);
    }
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    DIE(sockfd < 0, "socket");

    portno = atoi(argv[1]);
    DIE(portno == 0, "atoi");

    memset((char *)&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    int enable = 1;

    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int)) == -1) {
        perror("setsocketopt");
        exit(1);
    }
    ret = bind(sockfd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr));
    DIE(ret < 0, "bind");

    ret = listen(sockfd, MAX_CLIENTS);
    DIE(ret < 0, "listen");

    int exit_while = 1;

    user usr;
    while (exit_while) {
        // accept connection
        clilen = sizeof(cli_addr);
        newsockfd = accept(sockfd, (struct sockaddr *)&cli_addr, &clilen);
        DIE(newsockfd < 0, "accept");

        // receive data from this connection and parse them

        // this is what server received from client
        // need to check if is GET, POST, PUT or DELETE
        string response = receive(newsockfd);

        cout << response;

        memset(buffer, 0, BUFLEN);
        strcpy(buffer, response.c_str());
        // all lines from the request
        vector<string> lines = do_segment(buffer);

        // first line from the request received
        vector<string> first_line = tokenize(lines[0]);
        vector<string> second_line = tokenize(lines[1]);

        /* all requests have exactly 3 "words" separated by ' ' */
        /* request looks like GET /path HTTP/1.1 
                               0      1      2  */
        if (first_line.size() == REQUEST_SIZE &&
            !check_HTTP_version(first_line[2]) &&
            !check_host(second_line[1])) {

            if (!first_line[0].compare(GET_REQUEST)) {
                cout << "GET request\n";
                get_request(lines, first_line, newsockfd, catalog, cookies, users);

            } else if (!first_line[0].compare(POST_REQUEST)) {

                cout << "POST request\n";
                post_request(lines, first_line, newsockfd, catalog, cookies, users);

            } else if (!first_line[0].compare(PUT_REQUEST)) {
                cout << "PUT request\n";

                put_request(lines, first_line, newsockfd, catalog, cookies);

            } else if (!first_line[0].compare(DELETE_REQUEST)) {
                cout << "DELETE request\n";

                delete_request(lines, first_line, newsockfd, catalog, cookies);
            }
        } else {
            response = compute_response(BAD_REQUEST, HOSTNAME,
                                        CONTENT_TYPE_TEXT,
                                        "", "",
                                        "error:\"Wrong request\"\n");
            send_message(newsockfd, response.c_str());
        }

        close(newsockfd);
    }

    close(sockfd);

    return 0;
}