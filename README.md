
Small Python HTTP service that stores user accounts
locally and lets a C++ chess program create and validate logins.  
All calls use JSON over HTTP at **http://localhost:5050**.

Folder map
* login_service.py     – Python server  
* chess_users.json     – local user database 
* auth.hpp / auth.cpp  – C++ helpers that talk to server  

----------------------------------------------------------------------
How to run

1. Start server in its folder  
python login_service.py 

The console prints  
`Auth service running on http://localhost:5050`

2. Build app 
g++ -std=c++17 test_auth.cpp auth.cpp -lcurl -o test_auth

----------------------------------------------------------------------
All requests are HTTP POST with header  
`Content-Type: application/json`

Route list:
* **/register**  body `{ "username":"u", "password":"p" }`  
              reply `{ "status":"ok" }`
* **/login**     body `{ "username":"u", "password":"p" }`  
              reply `{ "status":"ok", "token":"<uuid>" }`
* **/validate**  body `{ "token":"<uuid>" }`  
              reply `{ "status":"ok", "valid": true | false }`


# Register
curl -X POST http://localhost:5050/register \
     -H "Content-Type: application/json" \
     -d '{"username":"user","password":"password123"}'

# Log in and capture token
token=$(curl -s -X POST http://localhost:5050/login \
        -H "Content-Type: application/json" \
        -d '{"username":"user","password":"password123"}' | jq -r .token)

# Validate token 
curl -X POST http://localhost:5050/validate \
     -H "Content-Type: application/json" \
     -d "{\"token\":\"$token\"}"
# returns  {"status":"ok","valid":true}

----------------------------------------------------------------------
