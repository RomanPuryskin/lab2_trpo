#pragma once
#include "ICipher.h"

// реализация шифрования на основе алгоритма AES-256-GCM

// AES - симметричный блочный шифр, то есть он использует один и тот же ключ для шифрования и расшифровки данных
// разбивая их на блоки фиксированного размера

// 256 - размер ключа в битах ( 32 байта )

// GCM - режим работы
class AesCipher : public ICipher {
    public:
        std::string encrypt(const std::string& data, const std::string& password) override;
        std::string decrypt(const std::string& data, const std::string& password) override;
};
