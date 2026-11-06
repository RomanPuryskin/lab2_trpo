#include "CryptoManager.h"
#include "AesCipher.h"
#include <fstream>
#include <iostream>
#include <sstream>
#include <stdexcept>

// в конструкторе создаем 
CryptoManager::CryptoManager() : cipher_(std::make_unique<AesCipher>()) {}

CryptoManager& CryptoManager::instance() {
    static CryptoManager inst;
    return inst;
}

void CryptoManager::encrypt(const std::filesystem::path& f, const std::string& p) { process(f, p, true); }
void CryptoManager::decrypt(const std::filesystem::path& f, const std::string& p) { process(f, p, false); }

void CryptoManager::process(const std::filesystem::path& folder, const std::string& password, bool encrypt) {
    if (!std::filesystem::is_directory(folder))
        throw std::runtime_error("папка не найдена: " + folder.string());

    for (const auto& file : walker_.files(folder)) {
        // открываем поток на чтение файла
        std::ifstream in(file, std::ios::binary);

        // записываем в буфер и закрываем потом на чтение
        std::ostringstream buf;
        buf << in.rdbuf();
        in.close();

        const std::string result = encrypt ? cipher_->encrypt(buf.str(), password)
                                           : cipher_->decrypt(buf.str(), password);

        // открываем потом на запись
        std::ofstream out(file, std::ios::binary | std::ios::trunc);
        out << result;

        // отдельно напечатаем результат
        std::cout << (encrypt ? "encrypted: " : "decrypted: ") << file.string() << '\n';
    }
}
