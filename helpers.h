#ifndef _HELPERS_
#define _HELPERS_

#include "general_includes.h"

// shows the current error
void error(const char *msg);

// adds a line to a string message
void compute_message(char *message, const char *line);

// opens a connection with server host_ip on port portno, returns a socket
int open_connection(const char *host_ip, int portno, int ip_type, int socket_type, int flag);

// closes a server connection on socket sockfd
void close_connection(int sockfd);

// send a message to a server
void send_message(int sockfd, const char *message);

// receives and returns the message from a server or client
char *receive(int sockfd);

// extracts and returns a JSON from a server response
char *basic_extract_json_response(char *str);

// gets hostame's ip address
void hostname_to_ip(const char *host_name, char *host_ip);

#endif
