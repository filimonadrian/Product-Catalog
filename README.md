# Product-Catalog

Installation
------------
It works only with linux. I've tested on Ubuntu 20.04 LTS.

Prerequisites
- C++11
- boost library:  
$ sudo apt install libboost-all-dev  
$ sudo apt install aptitude  
$ aptitude search boost  
  
Usage:
-----

Build and run server:  
$ make server && make run_server  
Build and run some tests:  
$ make test && make run_test  
  
For manual testing, i've used telnet.  
$ sudo apt-get install telnetd -y  
  
For all requests the "Content-Length = <value>" field is mandatory.
For now, all cookies are hardcoded -> all users has the same cookie.
The issue will be solved in future updates.


$ telnet localhost 12345

REGISTER  
POST /auth/register HTTP/1.1  
Host: localhost  
Content-Type: application/json  
Content-Length: 37    
  
{"username":"user","password":"pass"}  
  
LOGIN  
POST /auth/login HTTP/1.1   
Host: localhost  
Content-Type: application/json  
Content-Length: 37  

{"username":"user","password":"pass"}  

ADD_PRODUCT  
POST /catalog/products HTTP/1.1  
Host: localhost  
Cookie: cookie  
Content-Type: application/json  
Content-Length: 50  
  
{"name":"banana","category":"fruits","price":"10"}  

ALL PRODUCTS  
GET /catalog/products HTTP/1.1  
Host: localhost  
Cookie: cookie  
Content-Length: 0  
  
ONE PRODUCT  
GET /catalog/products/PRODUCT_ID HTTP/1.1  
Host: localhost  
Cookie: cookie  
Content-Length: 0  
  
DELETE  
DELETE /catalog/products/1 HTTP/1.1  
Host: localhost  
Cookie: cookie  
Content-Length: 0  
  
MODIFY  
PUT /catalog/products/1 HTTP/1.1  
Host: localhost  
Cookie: cookie  
Content-Length: 50  
  
{"name":"apple","category":"fruits","price":"11"}  
  
LOGOUT  
GET /auth/logout HTTP/1.1  
Host: localhost  
Cookie: cookie  
Content-Length: 0  
