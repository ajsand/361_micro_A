#include <iostream>
#include "auth.hpp"     

int main() {
    const std::string user = "demo";
    const std::string pass = "demo123";
    std::string token;

    /* 1. register */
    if (registerUser(user, pass))
        std::cout << "✔ registerUser OK\n";
    else
        std::cout << "• registerUser skipped (user may already exist)\n";

    /* 2. login and capture token */
    if (loginUser(user, pass, token))
        std::cout << "✔ loginUser OK   token = " << token << '\n';
    else {
        std::cout << "✖ loginUser failed\n";
        return 1;
    }

    /* 3. validate token */
    bool valid = validateToken(token);
    std::cout << "✔ validateToken returned " << (valid ? "true" : "false") << '\n';
    return 0;
}
