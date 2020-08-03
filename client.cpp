#include "client.h"
#include "path.h"

/* returns 0 if all is ok -> returned code is 2xx */
int response_code(string response) {
    std::stringstream ss(response);
    std::string line;

    if (!response.empty()) {
        getline(ss, line, '\n');
        cout << line << "\n";
        if (line[9] != '2') {
            return 1;
        }
    }
    return 0;
}

string extraction(string response, const char *start_find, const char *end_find) {
    if (response.empty()) {
        return "";
    }

    stringstream ss(response);
    string toFind;
    int start, end;
    if (response.find(start_find) == string::npos) {
        return "";
    }

    if (!strcmp(start_find, JSON_START)) {
        start = response.find(start_find);
    } else {
        start = response.find(start_find) + strlen(start_find);
    }

    if (start)
        end = response.find(end_find, start);
    return response.substr(start, end - start);
}

void print_book(json book) {
    for (json book : book) {
        cout << book << endl;
    }
}

int register_client(json credentials, int sockfd) {
    string message, response;
    message = compute_post_request(HOSTNAME, REGISTER, CONTENT_TYPE,
                                   credentials, "", "");

    send_message(sockfd, message.c_str());

    response = receive_from_server(sockfd);

    if (response_code(response) == 0) {
        return 0;
    }

    cout << extraction(response, ERR_START, ERR_END) << endl;
    return 1;
}

/* returns 0 if is OK */
int login(json credentials, int sockfd, string &cookie) {
    string message, response;
    message = compute_post_request(HOSTNAME, LOGIN, CONTENT_TYPE,
                                   credentials, "", "");

    send_message(sockfd, message.c_str());

    response = receive_from_server(sockfd);

    if (response_code(response) == 0) {
        cookie = extraction(response, COOKIE_START, COOKIE_END);
        return 0;
    }

    cout << extraction(response, ERR_START, ERR_END) << endl;

    return 1;
}

int logout(int sockfd, string cookie) {
    string message, response;
    message = compute_get_request(HOSTNAME, LOGOUT, GET, cookie, "");

    send_message(sockfd, message.c_str());

    response = receive_from_server(sockfd);

    if (response_code(response) == 0) {
        return 0;
    }

    cout << extraction(response, ERR_START, ERR_END) << endl;
    return 1;
}

int access_library(int sockfd, string cookie, string &jwt_token) {
    string message, response;

    message = compute_get_request(HOSTNAME, ACCESS, GET, cookie, "");

    send_message(sockfd, message.c_str());
    response = receive_from_server(sockfd);

    if (response_code(response) == 0) {
        jwt_token = extraction(response, JWT_START, JWT_END);
        return 0;
    }
    cout << extraction(response, ERR_START, ERR_END);
    return 1;
}

int add_book(int sockfd, string cookie, string jwt_token, json add_book_fields) {
    string message, response;

    message = compute_post_request(HOSTNAME, LIBRARY, CONTENT_TYPE,
                                   add_book_fields, cookie, jwt_token);

    send_message(sockfd, message.c_str());
    response = receive_from_server(sockfd);

    if (response_code(response) == 0) {
        jwt_token = extraction(response, JWT_START, JWT_END);
        return 0;
    }
    cout << extraction(response, ERR_START, ERR_END);
    return 1;
}

int library_information(int sockfd, string cookie, string jwt_token) {
    string message, response;

    message = compute_get_request(HOSTNAME, LIBRARY, GET, cookie, jwt_token);

    send_message(sockfd, message.c_str());
    response = receive_from_server(sockfd);

    if (response.empty()) {
        cout << "There is an error!\n";
        return 0;
    }

    if (response_code(response) == 0) {

        cout << extraction(response, JSON_START, JSON_END);
        return 0;
    }
    cout << extraction(response, ERR_START, ERR_END);
    return 1;
}

int book_information(int sockfd, string cookie, string jwt_token, int book_id) {
    string message, response;
    string lib_book = BOOK_INFO;
    lib_book = lib_book + to_string(book_id);

    message = compute_get_request(HOSTNAME, (lib_book.c_str()),
                                  GET, cookie, jwt_token);

    send_message(sockfd, message.c_str());
    response = receive_from_server(sockfd);

    if (response_code(response) == 0) {
        cout << extraction(response, JSON_START, JSON_END) << endl;
        return 0;
    } else {
        cout << extraction(response, ERR_START, ERR_END) << endl;
        return 1;
    }
}

int delete_book(int sockfd, string cookie, string jwt_token, int book_id) {
    string message, response;
    string lib_book = BOOK_INFO;
    lib_book = lib_book + to_string(book_id);

    message = compute_get_request(HOSTNAME, (lib_book.c_str()),
                                  DELETE, cookie, jwt_token);

    send_message(sockfd, message.c_str());
    response = receive_from_server(sockfd);

    if (response_code(response) == 0) {
        return 0;
    } else {

        cout << extraction(response, ERR_START, ERR_END) << endl;
        return 1;
    }
}

bool is_number(string s) {
    for (long unsigned int i = 0; i < s.length(); i++) {
        if (s[i] == '\n') {
            return true;
        }
        if (isdigit(s[i]) == false)
            return false;
    }

    return true;
}

int read_book_fields(json &add_book_fields) {
    char to_read[BUFREAD];
    int page_num = 0;
    memset(to_read, 0, BUFREAD);

    cout << "title = ";
    fgets(to_read, BUFREAD, stdin);
    fgets(to_read, BUFREAD, stdin);

    add_book_fields["title"] = strtok(to_read, "\n");
    cout << "author = ";
    fgets(to_read, BUFREAD, stdin);

    add_book_fields["author"] = strtok(to_read, "\n");
    cout << "genre = ";
    fgets(to_read, BUFREAD, stdin);

    add_book_fields["genre"] = strtok(to_read, "\n");
    cout << "publisher = ";
    fgets(to_read, BUFREAD, stdin);

    add_book_fields["publisher"] = strtok(to_read, "\n");

add_num:
    cout << "page_count = ";
    memset(to_read, 0, BUFREAD);
    fgets(to_read, BUFREAD, stdin);

    string str = strtok(to_read, "\n");

    if (!is_number(str)) {
        cout << "This is not a number!\n";
        goto add_num;
    }
    page_num = atoi(strtok(to_read, "\n"));
    add_book_fields["page_count"] = page_num;
    return 0;
}

int read_id(char to_read[BUFREAD]) {

    string str = strtok(to_read, "\n");
    if (!is_number(str)) {
        cout << "This is not an id!\n";
        return 1;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    int sockfd, ret;
    string cookie, jwt_token;
    char *host_ip;
    host_ip = (char *)malloc(20 * sizeof(char));
    char to_read[BUFREAD];
    int book_id;
    int online = 0;
    int access = 0;

    json credentials;
    json add_book_fields;

    hostname_to_ip(HOSTNAME, host_ip);
    sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);

    while (true) {
        memset(to_read, 0, sizeof(to_read));
        read(0, to_read, BUFREAD);

        if (!strcmp(to_read, "register\n") || !strcmp(to_read, "0\n")) {

            cout << "Register:\n";
            cout << "username=";
            cin >> to_read;
            credentials["username"] = to_read;
            cout << "password=";
            cin >> to_read;
            credentials["password"] = to_read;

            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);
            ret = register_client(credentials, sockfd);

            if (!ret) {
                cout << "Registered successfully!\n";
            }

            close_connection(sockfd);

        } else if (!strcmp(to_read, "login\n") || !strcmp(to_read, "1\n")) {

            cout << "Login:\n";
            if (online) {
                cout << "You are already logged in!\n";
                continue;
            }
            cout << "username=";
            cin >> to_read;
            credentials["username"] = to_read;
            cout << "password=";
            cin >> to_read;
            credentials["password"] = to_read;

            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);

            ret = login(credentials, sockfd, cookie);
            if (!ret) {
                online = 1;
            }
            close_connection(sockfd);

        } else if (!strcmp(to_read, "enter_library\n") ||
                   !strcmp(to_read, "2\n")) {
            cout << "Gain access:\n";
            if (!online) {
                cout << "You are not logged in!\n";
                continue;
            }

            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);

            ret = access_library(sockfd, cookie, jwt_token);
            if (!ret) {
                cout << "Now you have access to this library!\n";
                access = 1;
            }

            close_connection(sockfd);

        } else if (!strcmp(to_read, "get_books\n") || !strcmp(to_read, "3\n")) {
            cout << "All books details:\n";
            if (!online) {
                cout << "You are not logged in!\n";
                continue;
            }
            if (!access) {
                cout << "You don't have acces. Try: 2\n";
                continue;
            }

            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);
            ret = library_information(sockfd, cookie, jwt_token);
            if (!ret) {
                cout << "\n";
            }
            close_connection(sockfd);

        } else if (!strcmp(to_read, "get_book\n") || !strcmp(to_read, "4\n")) {

            cout << "Get book informations:\n";
            if (!online) {
                cout << "You are not logged in!\n";
                continue;
            }
            if (!access) {
                cout << "You don't have acces. Try: 2\n";
                continue;
            }
            cout << "id = ";
            cin >> book_id;
            // memset(to_read, 0, BUFREAD);
            // fgets(to_read, BUFREAD, stdin);
            // string str = to_read;
            // if (!is_number(str)) {
            //     cout << "This is not an id!\n";
            //     continue;
            // }

            // book_id = atoi(strtok(to_read, "\n"));

            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);
            ret = book_information(sockfd, cookie, jwt_token, book_id);
            if (!ret) {
                cout << "\n";
            }
            close_connection(sockfd);

        } else if (!strcmp(to_read, "add_book\n") || !strcmp(to_read, "5\n")) {
            cout << "Add book:\n";
            if (!online) {
                cout << "You are not logged in!\n";
                continue;
            }
            if (!access) {
                cout << "You don't have acces. Try: 2\n";
                continue;
            }
            read_book_fields(add_book_fields);

            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);
            ret = add_book(sockfd, cookie, jwt_token, add_book_fields);
            close_connection(sockfd);

        } else if (!strcmp(to_read, "delete_book\n") ||
                   !strcmp(to_read, "6\n")) {
            cout << "Delete book:\n";
            if (!online) {
                cout << "You are not logged in!\n";
                continue;
            }
            if (!access) {
                cout << "You don't have acces. Try: 2\n";
                continue;
            }
            cout << "id = ";
            cin >> book_id;
            // fgets(to_read, BUFREAD, stdin);

            // if (read_id(to_read)) {
            //     continue;
            // }
            // book_id = atoi(strtok(to_read, "\n"));

            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);
            ret = delete_book(sockfd, cookie, jwt_token, book_id);
            close_connection(sockfd);

        } else if (!strcmp(to_read, "logout\n") || !strcmp(to_read, "7\n")) {
            cout << "Log out:\n";
            if (!online) {
                cout << "You are not connected!\n";
                continue;
            }
            sockfd = open_connection(host_ip, PORT, AF_INET, SOCK_STREAM, 0);
            ret = logout(sockfd, cookie);
            if (!ret) {
                online = 0;
                access = 0;
                cookie = "";
                jwt_token = "";
            }

            close_connection(sockfd);

        } else if (!strcmp(to_read, "exit\n") || !strcmp(to_read, "-1\n")) {
            cout << "Bye-Bye!\n";
            break;
            // } else if (!strcmp(to_read, "\n")) {
            //   continue;
        } else {
            cout << "Invalid command! Try:\n";
            cout << "register         0\n"
                 << "login            1\n"
                 << "enter_library    2\n";
            cout << "get_books        3\n"
                 << "get_book         4\n"
                 << "add_book         5\n";
            cout << "delete_book      6\n"
                 << "logout           7\n"
                 << "exit            -1\n";
        }
    }

    close(sockfd);
    free(host_ip);
    return 0;
}
