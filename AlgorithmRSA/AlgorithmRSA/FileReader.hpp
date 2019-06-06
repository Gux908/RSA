#pragma once

#include <utility>
#include <type_traits>
#include <string>
#include <memory>
#include <inttypes.h>
#include <fstream>
#include <iostream>
#include <vector>

using data_t = uint16_t;

struct HeaderStructWAV
{
    unsigned char chunkID[4];     
    uint32_t chunkSize;           
    unsigned char format[4];      
    unsigned char subchunk1ID[4]; 
    uint32_t subchunk1Size;       
    uint16_t audioFormat;         
    uint16_t numChannels;
    uint32_t sampleRate; 
    uint32_t byteRate;
    uint16_t block_align;
    uint16_t bits_per_sample;
    unsigned char subchunk2ID[4]; 
    uint32_t subchunk2Size;       
};

void readWaveHeader(std::ifstream &file, HeaderStructWAV &Header);
data_t *readWAVDataFromFile(std::ifstream &file, HeaderStructWAV &Header);
data_t *modifyWAVData(data_t *Data, HeaderStructWAV &wav, int sampels_amount);
void writeWAVHeaderToFile(std::ofstream &file, HeaderStructWAV &wav);
void writeWAVDataToFile(std::ofstream &file, HeaderStructWAV &Header, data_t *WAVData);
void printHeader(const HeaderStructWAV &Header);
