#include "AesCipher.h"
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/gcm.h>
#include <cryptopp/osrng.h>
#include <cryptopp/pwdbased.h>
#include <cryptopp/sha.h>
#include <stdexcept>
#include <algorithm>

namespace {
    // все длины в байтах
    const size_t SALT = 16; // длина соли, cлучайный набор из 16 байт подмешивается к паролю, без соли один и тот же пароль всегда давал бы один и тот же ключ
    const size_t IV = 12; // случайные 12 байт, с которых режим GCM начинает работу, нужен для того, чтобы шифр по-разному преобразовывал одни и те же данные
    const size_t KEY = 32; // Длина ключа AES
    const size_t TAG = 16; // тега аутентификации, GCM дописывает в конец, При расшифровке GCM пересчитывает тег и сравнивает с записанным 
                            // Если он не совпал, значит либо пароль неверный либо файл изменили

    // превращение пароля в ключ для AES-256 (детерменировання)

    // SecByteBlock это буфер в Crypto++ для секретных данных
    // Он освобождает память затирая её нулями, чтобы ключ не остался в оперативной памяти после использования (в отличие от std::string)
    // byte в Crypto++ это unsigned char (мы передаем просто указатель на соль)
    CryptoPP::SecByteBlock makeKey(const std::string& password, const CryptoPP::byte* salt) {

        // DeriveKey принимает пароль как массив byte, а string хранит как массив char, поэтому создадим буфер и перекопируем туда пароль посимвольно
        CryptoPP::SecByteBlock pass(password.size());
        std::copy(password.begin(), password.end(), pass.begin());  // char -> byte

        // создали буфер для ключа, пока там 0 а потом запишется ключ из DeriveKey
        CryptoPP::SecByteBlock key(KEY);

        // выбираем алгоритм и вызываем функцию по генерации ключа
        CryptoPP::PKCS5_PBKDF2_HMAC<CryptoPP::SHA256> kdf;
        kdf.DeriveKey(key, key.size(), 0, pass, pass.size(), salt, SALT, 100000);


        return key;
    }
}  // локальный namespace

// метод принимает какое то содержимое и пароль, а возвращает строку которую можно записать на диск вместо исходных данных вида: соль | IV | шифртекст | тег
std::string AesCipher::encrypt(const std::string& data, const std::string& password) {

    // создаем генератор случайных чисел и генерируем соль и IV 
    CryptoPP::AutoSeededRandomPool rng;
    CryptoPP::SecByteBlock salt(SALT), iv(IV);
    rng.GenerateBlock(salt, salt.size());
    rng.GenerateBlock(iv, iv.size());

    // генерируем ключ
    CryptoPP::SecByteBlock key = makeKey(password, salt);

    // создаем шифратор и загружаем в него key и IV
    CryptoPP::GCM<CryptoPP::AES>::Encryption enc;
    enc.SetKeyWithIV(key, key.size(), iv, iv.size());

    // шифрование через конвейер:
    std::string body;

    //Crypto++ строит конвейер: StringSource (источник данных) ->  AuthenticatedEncryptionFilter (шифрование, в конце отдает тег) ->  StringSink (записывает в body)
    CryptoPP::StringSource(data, true, new CryptoPP::AuthenticatedEncryptionFilter(enc, new CryptoPP::StringSink(body)));

    // в out записываем соль и добавляем IV
    // для преобразования типов из byte* (которые дают буфера соли и IV) в char* ( которые нужны std::string )
    std::string out(reinterpret_cast<const char*>(salt.data()), SALT);
    out.append(reinterpret_cast<const char*>(iv.data()), IV);


    return out + body;
}

//принимает  зашифрованное содержимое и возвращает исходные данные
std::string AesCipher::decrypt(const std::string& data, const std::string& password) {

    // если данных в файле меньше самых необходимых, то бросаем ошибку
    if (data.size() < SALT + IV + TAG)
        throw std::runtime_error("файл не является зашифрованным");

    // создаем указатель на начало данных ( начало это будет соль , поэтому дальше в makeKey эту полученную соль и передаем) 
    const CryptoPP::byte* p = reinterpret_cast<const CryptoPP::byte*>(data.data());
    CryptoPP::SecByteBlock key = makeKey(password, p);

    // аналогично создаем дешифратор и передаем в него key и IV ( IV будет p + SALT)
    CryptoPP::GCM<CryptoPP::AES>::Decryption dec;
    dec.SetKeyWithIV(key, key.size(), p + SALT, IV);


    // аналогично создаем конвеер для дешифровки
    std::string plain;
    try {
        CryptoPP::StringSource(data.substr(SALT + IV), true, new CryptoPP::AuthenticatedDecryptionFilter(dec, new CryptoPP::StringSink(plain)));
    } catch (const CryptoPP::HashVerificationFilter::HashVerificationFailed&) {
        throw std::runtime_error("неверный пароль или файл повреждён");
    }
    return plain;
}
