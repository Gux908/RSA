#include "FileReader.hpp"
#include "pch.h"

void printHeader(const HeaderStructWAV &Header)
{
    using namespace std;
    cout << "Header of *.wav file :" << endl;
    cout << "\t\t\tChunkID           " << Header.chunkID[0] << Header.chunkID[1] << Header.chunkID[2] << Header.chunkID[3] << endl;
    cout << "\t\t\tChunkSize         " << Header.chunkSize << endl;
    cout << "\t\t\tFormat            " << Header.format[0] << Header.format[1] << Header.format[2] << Header.format[3] << endl;
    cout << "\t\t\tSubchunk1ID       " << Header.subchunk1ID[0] << Header.subchunk1ID[1] << Header.subchunk1ID[2] << Header.subchunk1ID[3] << endl;
    cout << "\t\t\tSubchunk1Size     " << Header.subchunk1Size << endl;
    cout << "\t\t\tAudioFormat       " << Header.audioFormat << endl;
    cout << "\t\t\tNumChannels       " << Header.numChannels << endl;
    cout << "\t\t\tSampleRate        " << Header.sampleRate << endl;
    cout << "\t\t\tByteRate          " << Header.byteRate << endl;
    cout << "\t\t\tBlock_align       " << Header.block_align << endl;
    cout << "\t\t\tBits_per_sample   " << Header.bits_per_sample << endl;
    cout << "\t\t\tSubchunk2ID       " << Header.subchunk2ID[0] << Header.subchunk2ID[1] << Header.subchunk2ID[2] << Header.subchunk2ID[3] << endl;
    cout << "\t\t\tSubchunk2Size     " << Header.subchunk2Size << endl;
}


void readWaveHeader(std::ifstream &file, HeaderStructWAV &Header)
{
    int sampleAmount = sizeof(HeaderStructWAV);
    char *readedHeader = new char[sampleAmount];
    file.read(readedHeader, sampleAmount);
    HeaderStructWAV *wav = reinterpret_cast<HeaderStructWAV *>(readedHeader);
    Header = *wav;
}

data_t *readWAVDataFromFile(std::ifstream &file, HeaderStructWAV &Header)
{
    int dataAmount = Header.subchunk2Size;
    char *dataTable = new char[dataAmount];
    file.read(dataTable, dataAmount);
    data_t *readedData = reinterpret_cast<data_t *>(dataTable);
    file.close();
    return readedData;
}

void writeWAVHeaderToFile(std::ofstream &file, HeaderStructWAV &wav)
{
    HeaderStructWAV header = wav;
    char *buffer = reinterpret_cast<char *>(&header);
    file.write(buffer, sizeof(HeaderStructWAV));
}

void writeWAVDataToFile(std::ofstream &file, HeaderStructWAV &Header, data_t *WAVData)
{
    int data_size = Header.subchunk2Size;
    char *buffer = reinterpret_cast<char *>(WAVData);
    file.write(buffer, data_size);
}

//function modifi first half part of wav file
data_t *modifyWAVData(data_t *Data, HeaderStructWAV &wav, int sampels_amount)
{
    if (wav.numChannels == 2)
    {
        for (int i = 0; i < (sampels_amount / 4); i++)
        {
            if (i % 2 == 0)
                Data[i] = 200 + (i % 103) * 2;
            else
                Data[i] = -330 + ((i + 50) % 111) * 1.4;
        }
    }
    else
    {
        for (int i = 0; i < sampels_amount / 4; i++)
        {
            Data[i] = 1000;
        }
    }
    return Data;
}
