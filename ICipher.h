#pragma once
#include <string>

class ICipher {
    public:
        virtual ~ICipher() = default;
        virtual std::string encrypt(const std::string& data, const std::string& password) = 0;
        virtual std::string decrypt(const std::string& data, const std::string& password) = 0;
};
