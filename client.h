#ifndef CLIENT_H
#define CLIENT_H
#include "general_includes.h"

#define PORT 8080
#define BUFREAD 100

const char *ERR_START = "error\":\"";
const char *ERR_END = "\"";
const char *COOKIE_START = "Cookie: ";
const char *COOKIE_END = ";";
const char *JWT_START = "token\":\"";
const char *JWT_END = "\"";
const char *JSON_START = "[{";
const char *JSON_END = "]}";

#endif