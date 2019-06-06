#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <inttypes.h>
#include <utility>
#include <type_traits>
#include <random>
#include <algorithm>

#include <boost/multiprecision/cpp_int.hpp>
#include <boost/numeric/conversion/cast.hpp>

#include "menu.hpp"
#include "Cryptography.hpp"
#include "FileReader.hpp"

#include "pch.h"

void displayManu(void)
{
    std::cout << "==========================================" << std::endl;
    std::cout << "|    Rsa Alogorytm                        |" << std::endl;
    std::cout << "==========================================" << std::endl;
    std::cout << "|    k-> RSA kodowanie                    |" << std::endl;
    std::cout << "|    d-> RSA dekodowanie                  |" << std::endl;
    std::cout << "|    t-> Test kodowania i dekodowania RSA |" << std::endl;
    std::cout << "|    l-> Wczytaj plik WAV                 |" << std::endl;
    std::cout << "|    s-> Zapisz do pliku WAV              |" << std::endl;
    std::cout << "|    q-> Quit                             |" << std::endl;
    std::cout << "==========================================" << std::endl;
}

void Menu()
{
    char switchOption = 'z';
    HeaderStructWAV headerWAV;
    data_t *dataWAV = nullptr;

    std::string sourceNameFileWAV = "";
    std::string saveNameFileWAV = "";
    std::ifstream sourceFileWAV(sourceNameFileWAV.c_str(), std::ifstream::binary);
    std::ofstream saveFileWAV(saveNameFileWAV.c_str(), std::ofstream::binary);

    Cryptography::RsaKeys<uint32_t> keys(61, 101);

    int dataAmount=0;

    do
    {
        displayManu();
        std::cin >> switchOption;
        switch (switchOption)
        {
        case 'l':
        {
            std::string input;
            std::cout << "Plik: \n";
            std::cin >> input;
            sourceNameFileWAV = input;
            std::ifstream loadFile(sourceNameFileWAV.c_str(), std::ifstream::binary);

            if (!loadFile)
            {
                return;
            }
            readWaveHeader(loadFile, headerWAV);
            printHeader(headerWAV);

            dataAmount = headerWAV.subchunk2Size;
            dataWAV = new data_t[dataAmount]();
            dataWAV = readWAVDataFromFile(loadFile, headerWAV);

            sourceNameFileWAV.clear();
        }
        break;
        case 's':
        {
            std::string input;
            std::cout << "Zapisz do: \n";
            std::cin >> input;
            saveNameFileWAV = input;
            std::ofstream file_save(saveNameFileWAV.c_str(), std::ofstream::binary);

            if (!file_save)
            {
                break;
            }

            if (std::is_empty<HeaderStructWAV>::value) // nie mozna zapisac bez headera
            {
                file_save.close();
                break;
            }

            writeWAVHeaderToFile(file_save, headerWAV);
            writeWAVDataToFile(file_save, headerWAV, dataWAV);

            saveNameFileWAV.clear();
        }
        break;
        case 't':
        {
            std::vector<data_t> data;
            data.resize(20);
            std::iota(data.begin(), data.end(), 1);

            keys.generateKeys(131, 107);
            keys.PrintKeys();
            auto encrypted = Cryptography::encryptAlgorithmRSA(data, keys);
            auto decrypted = Cryptography::decryptAlgorithmRSA(encrypted, keys);
            std::cout << "Data: \n";
            for (auto x : data)
            {
                std::cout << x << " ";
            }
            std::cout << "\n";
            std::cout << "Encrypted: \n";
            for (auto x : encrypted)
            {
                std::cout << x << " ";
            }
            std::cout << "\n";
            std::cout << "Decrypted: \n";
            for (auto x : decrypted)
            {
                std::cout << x << " ";
            }
            std::cout << "\n";
        }
        case 'k':
        {
            std::vector<data_t> vec;
            vec.resize(dataAmount);

            for (int i = 0; i < dataAmount; i++)
            {
                vec[i] = *(dataWAV + i);
            }

            vec = Cryptography::encryptAlgorithmRSA(vec, keys);

            std::ofstream RSA_p_file("rsaPKey.txt");
            RSA_p_file << keys.pValue;
            RSA_p_file.close();

            std::ofstream RSA_q_file("rsaQKey.txt");
            RSA_q_file << keys.qValue;
            RSA_q_file.close();

            dataWAV = new data_t[dataAmount]();

            for (int j = 0; j < dataAmount; j++)
            {
                *(dataWAV + j) = vec[j];
            }

            vec.clear();
            vec.shrink_to_fit();
        }
        break;
        case 'd':
        {
            std::vector<data_t> vec;
            vec.resize(dataAmount);

            for (int i = 0; i < dataAmount; i++)
            {
                vec[i] = *(dataWAV + i);
            }

            std::ifstream RSA_p_file("rsa_p_key.txt");
            RSA_p_file >> keys.pValue;
            std::cout << "keys.pValue : " << keys.pValue << std::endl;
            RSA_p_file.close();

            std::ifstream RSA_q_file("rsa_q_key.txt");
            RSA_q_file >> keys.qValue;
            std::cout << "keys.qValue : " << keys.qValue << std::endl;
            RSA_q_file.close();

            vec = Cryptography::decryptAlgorithmRSA(vec, keys);

            dataWAV = new data_t[dataAmount](); 

            for (int j = 0; j < dataAmount / 2; j++)
            {
                *(dataWAV + j) = vec[j];
            }

            vec.clear();         
            vec.shrink_to_fit(); 
        }
        break;
            break;
        case 'q':
            return;
        default:
            std::cout << "Wybierz coœ innego\n";
            break;
        }
    } while (switchOption != 'q');
	// czyszczenie tablicy
	delete dataWAV;
}