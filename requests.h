#ifndef _REQUESTS_
#define _REQUESTS_


char *compute_get_request(const char *host, const char *url,
                          int flag, string cookies, string jwt_token);

char *compute_post_request(const char *host, const char *url,
                           const char *content_type,
                           json body_data,
                           string cookies,
                           string jwt_token);

#endif
