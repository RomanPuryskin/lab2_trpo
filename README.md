# foldercrypt

Консольное шифрование/дешифрование папок (рекурсивно). C++17, Crypto++, AES-256-GCM, PBKDF2-HMAC-SHA256

## Сборка
    cmake -B build && cmake --build build

## Запуск
    ./foldercrypt encrypt <папка> <пароль>
    ./foldercrypt decrypt <папка> <пароль>

## Архитектура
AesCipher — только шифр; Pbkdf2KeyDerivation — только KDF; FsDirectoryWalker — только обход; CryptoManager — оркестрация.
- Singleton: CryptoManager::instance().
