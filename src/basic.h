#pragma once
#include <vector>
#include <iostream>
#include <map>

class Basic {
public:
    ~Basic();

protected:
    static const int MAX_CHAR = 261;
    static const int BLOCK_SIZE = 1024;
    const int BITS_ = 8;
    const int BITS_ADVANCED_ = 9;
    const int FILENAME_END_ = 256;
    const int ONE_MORE_FILE_ = 257;
    const int ARCHIVE_END_ = 258;
    int char_len_[MAX_CHAR];
    int cnt_len_[MAX_CHAR];
    int code_map_[MAX_CHAR];

    unsigned char char_buffer_ = 0;
    int char_pointer_ = 0;
    char *writing_block_ = new char[BLOCK_SIZE];
    char *reading_block_ = new char[BLOCK_SIZE];
    int block_pointer_ = 0;
    void ClearFile(char *file_name) const;
    void ClearFile(std::string &file_name) const;
};
