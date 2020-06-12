#include "helpers.h"

char *compute_response(const char *http_code,
                           const char *server,
                           const char *content_type,
                           string cookies,
                           string jwt_token,
                           string data) {

    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    string help_query, help_cookie;
    unsigned long size = 0;
    if (data.size() != 0) {
        size = data.size();
    } else if (cookies.size() != 0) {
        size = cookies.size();
    } else if (jwt_token.size() != 0) {
        size = jwt_token.size();
    }

    // write the method name, URL, request params (if any) and protocol type

    /* http_code contains first line: version and result 
    Ex : HTTP/1.1 200 OK */
    sprintf(line, "%s", http_code);
    compute_message(message, line);

    sprintf(line, "Server: %s", server);
    compute_message(message, line);

    /* current date and time*/
    time_t current_time;
    time(&current_time);
    string c = ctime(&current_time);
    c.pop_back();
    sprintf(line, "Date: %s", c.c_str());
    compute_message(message, line);

    /* content type: application json or text */
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    /* content length: application json or text */
    sprintf(line, "Content-Length: %ld", size);
    compute_message(message, line);

    /* content length: application json or text */
    sprintf(line, "Connection: close");
    compute_message(message, line);

    // add new line at end of header
    compute_message(message, "");

    /* add data in the response: error, cookie, jwt token, json with products */
    if (!cookies.empty()) {
        sprintf(line, "{\"%s\"}", cookies.c_str());
        compute_message(message, line);

    } else if (!jwt_token.empty()) {
        sprintf(line, "{\"%s\"}", jwt_token.c_str());
        compute_message(message, line);

    } else if (!data.empty()) {
        compute_message(message, data.c_str());
    }

    free(line);
    return message;
}
