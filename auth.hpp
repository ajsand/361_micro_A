#ifndef AUTH_HPP
#define AUTH_HPP
#include <string>

// returns true on success and fills token
bool loginUser(const std::string& username,
                const std::string& password,
                std::string& outToken);

bool registerUser(const std::string& username,
                const std::string& password);

bool validateToken(const std::string& token);

#endif
