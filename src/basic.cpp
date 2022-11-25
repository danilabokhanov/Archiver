#include "basic.h"
#include <fstream>

void Basic::ClearFile(char* file_name) const {
    std::ofstream output(file_name, std::ios::out | std::ios::binary);
    output.write(writing_block_, 0);
    output.close();
}

void Basic::ClearFile(std::string& file_name) const {
    std::ofstream output(file_name, std::ios::out | std::ios::binary);
    output.write(writing_block_, 0);
    output.close();
}

Basic::~Basic() {
    delete[] reading_block_;
    delete[] writing_block_;
}
