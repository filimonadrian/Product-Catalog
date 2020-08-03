#include "client.h"

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
    message = compute_post_request(HOSTNAME, REGISTER, CONTENT_TYPE, credentials, "", "");

    send_to_server(sockfd, message.c_str());

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
    message = compute_post_request(HOSTNAME, LOGIN, CONTENT_TYPE, credentials, "", "");

    send_to_server(sockfd, message.c_str());

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

    send_to_server(sockfd, message.c_str());

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

    send_to_server(sockfd, message.c_str());
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

    message = compute_post_request(HOSTNAME, LIBRARY, CONTENT_TYPE, add_book_fields,
                                   cookie, jwt_token);

    send_to_server(sockfd, message.c_str());
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

    send_to_server(sockfd, message.c_str());
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

    send_to_server(sockfd, message.c_str());
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

    send_to_server(sockfd, message.c_str());
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
