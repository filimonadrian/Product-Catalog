#ifndef SERVER_HELPER_H
#define SERVER_HELPER_H

#include "general_includes.h"
#define WRONG_PATH 0
#define ENTIRE_PATH 1
#define RESOURCE_PATH 2

extern const char *HOSTNAME;
extern const char *ROOT;
extern const char *CONTENT_TYPE;
extern const char *REGISTER;
extern const char *LOGIN;
extern const char *LOGOUT;
extern const char *ACCESS;
extern const char *CATALOG;

extern const char *PRODUCT_INFO;

const char *SERVER_TEST = "GET / HTTP/1.1/r/n";
const char *GET_PRODUCTS = "GET /products HTTP/1.1/r/n";

const char *HTTP_VERSION0 = "HTTP/1.0\r";
const char *HTTP_VERSION1 = "HTTP/1.1\r";
const char *GET_REQUEST = "GET";
const char *POST_REQUEST = "POST";
const char *PUT_REQUEST = "PUT";
const char *DELETE_REQUEST = "DELETE";

int product_id = 1;

std::set<string> paths_of_get;
std::set<string> paths_of_post;

typedef struct product {
    int id;
    int price;
    string name;
    string category;
    string created_date;
    string updated_date;
} product;

typedef struct user {
    string username;
    string password;
    string cookie;
} user;

void usage(char *file);
void close_all_sock(fd_set *fd, int fdmax);
vector<string> tokenize(string comm);
vector<string> tokenize(string comm, const char escape[]);

void send_products(unordered_map<int, product> catalog);
vector<string> do_segment(char *sentence);

int check_HTTP_version(string to_test);
void get_request(vector<string> lines, vector<string> first_line, int sockfd,
                 unordered_map<int, product> &catalog,
                 unordered_map<string, user> &cookies,
                 unordered_map<string, user> &users);
void post_request(vector<string> lines, vector<string> first_line, int sockfd,
                  unordered_map<int, product> &catalog,
                  unordered_map<string, user> &cookies,
                  unordered_map<string, user> &users);

void put_request(vector<string> lines, vector<string> first_line, int sockfd,
                 unordered_map<int, product> &catalog,
                 unordered_map<string, user> &cookies);

void delete_request(vector<string> lines, vector<string> first_line, int sockfd,
                    unordered_map<int, product> &catalog,
                    unordered_map<string, user> &cookies);

char *not_found(char *message);
int check_path_exists(vector<string> lines, set<string> paths);
int get_id(vector<string> first_line);
int check_number(string data);

int register_user(string data, unordered_map<string, user> &users);
int login(string data, unordered_map<string, user> &users,
          unordered_map<string, user> &cookies, string &cookie);
int logout(unordered_map<string, user> &users,
           unordered_map<string, user> &cookies,
           string cookie);

int add_product(string data, unordered_map<int, product> &catalog);
int modify_product(string data, unordered_map<int, product> &catalog, int id);
int delete_product(unordered_map<int, product> &catalog, int id);

#endif