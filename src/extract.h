#pragma once
#include <vector>
#include <iostream>
#include <fstream>
#include <map>
#include "basic.h"

class Extract : public Basic {
public:
    ~Extract();
    explicit Extract(const char* compressed_file);
    void ExtractAll();

private:
    int ReadNBitsNumber(int n);
    int GetBit();
    void ReadChar();
    void ReadBlock();
    void BuildCodeMap();
    void WriteInFile();
    void WriteChar(char ch);
    void Reset();
    int DecodeChar();
    std::ifstream input_;
    int writing_block_pointer_ = 0;
    std::string extracted_file_;
    int compressed_bor_[MAX_CHAR][MAX_CHAR];
};
