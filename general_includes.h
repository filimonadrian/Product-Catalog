#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <sys/types.h>
#include <unistd.h> /* read, write, close */

#include "nlohmann/json.hpp"
#include <bits/stdc++.h>
#include <iostream>
#include <string>
#include <unordered_map>
#include <set>
#include <boost/algorithm/string.hpp>

#define BUFLEN 5096
#define LINELEN 1000
#define DELETE 1
#define GET 0
using json = nlohmann::json;
/*
 * Macro de verificare a erorilor
 * Exemplu:
 *     int fd = open(file_name, O_RDONLY);
 *     DIE(fd == -1, "open failed");
 */
#define DIE(assertion, call_description)	\
	do {									\
		if (assertion) {					\
			fprintf(stderr, "(%s, %d): ",	\
					__FILE__, __LINE__);	\
			perror(call_description);		\
			exit(EXIT_FAILURE);				\
		}									\
	} while(0)


using namespace std;