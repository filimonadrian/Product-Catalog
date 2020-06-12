#ifndef _RESPONSE_
#define _RESPONSE_

#define MAX_CLIENTS 100
#define REQUEST_SIZE 3

const char *SERVER = "localhost";

const char *OK = "HTTP/1.1 200 OK";
const char *CREATED = "HTTP/1.1 201 Created";
const char *NO_CONTENT = "HTTP/1.1 204 No Content";
const char *BAD_REQUEST = "HTTP/1.1 400 Bad Request";
const char *UNAUTHORIZED = "HTTP/1.1 401 Unauthorized";
const char *FORBIDDEN = "HTTP/1.1 403 Forbidden";
const char *NOT_FOUND = "HTTP/1.1 404 Not Found";
const char *SERVER_ERROR = "HTTP/1.1 500 Internal Server Error";

char *compute_response(const char *http_code,
                           const char *server,
                           const char *content_type,
                           string cookies,
                           string jwt_token,
                           string data);
#endif
