#include "CryptoManager.h"
#include <iostream>
#include <string>

int main(int argc, char** argv) {
    if (argc != 4 || (std::string(argv[1]) != "encrypt" && std::string(argv[1]) != "decrypt")) {
        std::cerr << "Использование: " << argv[0] << " <encrypt|decrypt> <папка> <пароль>\n";
        return 2;
    }
    try {
        auto& manager = CryptoManager::instance();
        
        if (std::string(argv[1]) == "encrypt") {
            manager.encrypt(argv[2], argv[3]);
        } else {
            manager.decrypt(argv[2], argv[3]);
        }                                 
    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << '\n';
        return 1;
    }
}
