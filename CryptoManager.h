#pragma once
#include "DirectoryWalker.h"
#include "ICipher.h"
#include <memory>

// Singleton: шифрование/дешифрование папки со всеми вложенными файлами.
class CryptoManager {
    public:
        // статический метод, через который получим единственный экземпляр ( он вернет ссылку а не копию )
        static CryptoManager& instance();

        // необходимо для единственного экземпляра, 
        CryptoManager(const CryptoManager&) = delete; //запрещаем явное копирование 
        CryptoManager& operator=(const CryptoManager&) = delete; // запрещаем явное присваивание


        //методы не возвращат ничего явно, результат это изменыенные файлы
        void encrypt(const std::filesystem::path& folder, const std::string& password);
        void decrypt(const std::filesystem::path& folder, const std::string& password);

    private:
        // конструктор приватный, так как не сможем просто создать еще один екземпляр класса
        CryptoManager();

        void process(const std::filesystem::path& folder, const std::string& password, bool encrypt);

        // указатель уникальный, чтобы нельзя было скопировать ( согласуется с запретом на копирование менеджера)
        std::unique_ptr<ICipher> cipher_;
        DirectoryWalker walker_;
};
