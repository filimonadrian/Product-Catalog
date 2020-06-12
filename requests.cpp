#include "helpers.h"

char *compute_get_request(const char *host, const char *url,
                          int flag, string cookies, string jwt_token) {

    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    string help_query, help_cookie;

    // write the method name, URL, request params (if any) and protocol type

    if (flag == DELETE) {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // add the host
    if (host != NULL) {
        sprintf(line, "Host: %s", host);
    }
    compute_message(message, line);

    // add headers and/or cookies, according to the protocol format

    if (!cookies.empty()) {
        sprintf(line, "Cookie: %s", cookies.c_str());
        compute_message(message, line);
    }

    // add jwt token
    if (!jwt_token.empty()) {
        sprintf(line, "Authorization: Bearer %s", jwt_token.c_str());
        compute_message(message, line);
    }

    sprintf(line, "Content-Length: %d", 0);
    compute_message(message, line);

    // add final new line
    compute_message(message, "");
    free(line);
    return message;
}

char *compute_post_request(const char *host, const char *url,
                           const char *content_type,
                           json body_data,
                           string cookies,
                           string jwt_token) {

    char *message = (char *)calloc(BUFLEN, sizeof(char));
    char *line = (char *)calloc(LINELEN, sizeof(char));
    string help_body, help_cookie;
    help_body = body_data.dump();

    if (body_data == NULL)
        help_body = "";
    else
        help_body = body_data.dump();

    // write the method name, URL and protocol type
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // add the host
    if (host != NULL) {
        sprintf(line, "Host: %s", host);
    }
    compute_message(message, line);

    /* add necessary headers (Content-Type and Content-Length are mandatory)
        in order to write Content-Length you must first compute the message size
    */

    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);

    sprintf(line, "Content-Length: %ld", help_body.size());
    compute_message(message, line);

    // add cookies
    if (!cookies.empty()) {
        sprintf(line, "Cookie: %s", help_cookie.c_str());
        compute_message(message, line);
    }
    //add jwt token
    if (!jwt_token.empty()) {
        sprintf(line, "Authorization: Bearer %s", jwt_token.c_str());
        compute_message(message, line);
    }

    // add new line at end of header
    compute_message(message, "");

    // add the actual payload data
    compute_message(message, help_body.c_str());

    free(line);
    return message;
}
