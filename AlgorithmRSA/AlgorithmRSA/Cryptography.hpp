#pragma once

#include <iostream>
#include <vector>
#include <cmath>
#include <cinttypes>
#include <cstdint>
#include <tuple>
#include <thread>
#include <functional>
#include <random>
#include <numeric>
#include <algorithm>
#include <cstdlib>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/numeric/conversion/cast.hpp>
#include <boost/integer.hpp>

using namespace boost::multiprecision;

namespace Cryptography
{
using numeric_t = uint16_t; // typ numeryczny w wave file
using uint128_t = boost::multiprecision::uint128_t;
using uint1024_t = boost::multiprecision::uint1024_t;
using uint4096_t = number<cpp_int_backend<65536, 65536, unsigned_magnitude, unchecked, void>>;
using uint_max_t = number<cpp_int_backend<65536, 65536, unsigned_magnitude, unchecked, void>>;

// Euclidean algorithm
template <typename key_t>
key_t greatestCommonDenominator(key_t firstKey, key_t secondKey)
{
    key_t denominator;

    while (true)
    {
        denominator = firstKey % secondKey;
        if (denominator == 0)
        {
            return secondKey;
        }
        firstKey = secondKey;
        secondKey = denominator;
    }
}

template <typename key_t>
key_t getCoPrime(key_t phiValue)
{
    key_t exponent = 2; 
    while (exponent < phiValue)
    {
        if (greatestCommonDenominator(exponent, phiValue) == 1)
        {
            return exponent;
        }
        else
        {
            exponent++;
        }
    }

    return exponent;
}

template <typename key_t>
struct RsaKeys
{
    key_t pValue;
    key_t qValue;
    key_t publicKey;
    key_t exponent;
    key_t privateKey;

    RsaKeys() = default;
    RsaKeys(key_t p, key_t q)
    {
        this->pValue = p;
        this->qValue = q;
        this->publicKey = pValue * qValue;
        this->exponent = getCoPrime((pValue - 1) * (qValue - 1));
        this->privateKey = (1 + (2 * (pValue - 1) * (qValue - 1))) / (exponent);
    }

    ~RsaKeys(){};

    void PrintKeys(void)
    {
        std::cout << "Prime numbers: " << this->pValue << " " << this->qValue << "\n";
        std::cout << "Private Key: " << this->privateKey << "\n";
        std::cout << "Public Key: " << this->publicKey << "\n";
        std::cout << "Exponent: " << this->exponent << "\n";
    }

    void generateKeys(key_t p, key_t q)
    {
        this->pValue = p;
        this->qValue = q;
        this->publicKey = pValue * qValue;
        this->exponent = getCoPrime((pValue - 1) * (qValue - 1));
        this->privateKey = (1 + (2 * (pValue - 1) * (qValue - 1))) / (exponent);
    }
};


template <typename key_t>
key_t raiseLargeNumber(numeric_t number, key_t exponent)
{
    key_t large_number = boost::numeric_cast<key_t>(number);

    for (unsigned int i = 1; i < exponent; i++)
    {
        large_number *= number;
    }

    return large_number;
}

template <typename key_t, typename numeric_t>
key_t modularExponent(numeric_t number, key_t exponent, key_t modulus)
{
    if (modulus == 1)
    {
        return 0;
    }

    else
    {
        key_t result = 1;
        for (unsigned int i = 0; i < exponent; i++)
        {
            result = (result * number) % modulus;
        }

        return boost::numeric_cast<key_t>(result); 
    }
}

template <typename key_t>
numeric_t decryptingExponent(key_t number, key_t exponent, key_t modulus)
{
    if (modulus == 1)
    {
        return 0;
    }

    else
    {
        uint4096_t result = 1;
        for (unsigned int i = 0; i < exponent; i++)
        {
            result = (result * number) % modulus;
        }

        return (numeric_t)result; 
    }
}

template <typename key_t>
std::vector<numeric_t> encryptAlgorithmRSA(std::vector<numeric_t> data, RsaKeys<key_t> keys)
{
    std::vector<key_t> encryptedData;
    std::vector<numeric_t> solutionData;

    for (auto element : data)
    {
        key_t encrypted_number = modularExponent(element, keys.exponent, keys.publicKey);
        encryptedData.push_back(encrypted_number);
    }

    for (auto element : encryptedData)
    {
        for (unsigned int i = 0; i < sizeof(key_t) / sizeof(numeric_t); ++i)
        {
            numeric_t value = (element >> (8 * sizeof(numeric_t) * i));
            solutionData.push_back(value);
        }
    }
    return solutionData;
}

template <typename key_t>
std::vector<numeric_t> decryptAlgorithmRSA(std::vector<numeric_t> data, RsaKeys<key_t> keys)
{
    std::vector<numeric_t> decryptedData;
    auto sizeRatio = sizeof(key_t) / sizeof(numeric_t);
    std::vector<key_t> encryptedData(data.size() * sizeof(numeric_t) / sizeof(key_t), 0);

    for (unsigned int i = 0; i < encryptedData.size(); ++i)
    {
        for (unsigned int j = 0; j < sizeRatio; ++j)
        {
            encryptedData[i] += j == 0 ? data[sizeRatio * i + j] : (key_t)data[sizeRatio * i + j] * raiseLargeNumber(2, 8 * sizeRatio * j);
        }
    }

    for (auto element : encryptedData)
    {
        numeric_t decrypted_number = decryptingExponent(element, keys.privateKey, keys.publicKey);
        decryptedData.push_back(decrypted_number);
    }

    return decryptedData;
}

// Miller-Rabin test
template <typename key_t>
bool millerRabinTest(key_t number, key_t rounds)
{
    key_t random = 2 + rand() % (number - 4);
    key_t x = modularExponent(random, rounds, number);

    if (x == 1 || x == number - 1)
    {
        return true;
    }

    while (rounds != number - 1)
    {
        x = (x * x) % number;
        rounds *= 2;

        if (x == 1)
        {
            return false;
        }
        if (x == number - 1)
        {
            return true;
        }
    }

    return false;
}

template <typename key_t>
key_t generatePrime()
{
    key_t number = 0;

    number ^= 1UL;                       
    number ^= 1UL << 4 * sizeof(number); 

    while (!millerRabinTest(number))
    {
        number--;
    }

    return number;
}
} // namespace Cryptography
