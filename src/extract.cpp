#include <vector>
#include <iostream>
#include "extract.h"
#include "exception.h"

Extract::Extract(const char *compressed_file) : input_(compressed_file, std::ios::in | std::ios::binary) {
    char_pointer_ = -1;
    block_pointer_ = BLOCK_SIZE;
}

Extract::~Extract() {
    input_.close();
}

void Extract::ReadBlock() {
    input_.read(reading_block_, BLOCK_SIZE);
}

void Extract::ReadChar() {
    if (block_pointer_ == BLOCK_SIZE) {
        ReadBlock();
        block_pointer_ = 0;
    }
    char_buffer_ = reading_block_[block_pointer_++];
}

int Extract::GetBit() {
    if (char_pointer_ == -1) {
        ReadChar();
        char_pointer_ = BITS_ - 1;
    }
    return ((char_buffer_ >> (char_pointer_--)) & 1);
}

int Extract::ReadNBitsNumber(int n) {
    int res = 0;
    for (int i = 0; i < n; i++) {
        res <<= 1;
        res += GetBit();
    }
    return res;
}

void Extract::BuildCodeMap() {
    int symbols_cnt = ReadNBitsNumber(BITS_ADVANCED_);
    std::vector<int> symbols_permutation(symbols_cnt);
    for (int i = 0; i < symbols_cnt; i++) {
        symbols_permutation[i] = ReadNBitsNumber(BITS_ADVANCED_);
    }
    reverse(symbols_permutation.begin(), symbols_permutation.end());
    std::fill(cnt_len_, cnt_len_ + MAX_CHAR, 0);
    int index = 1;
    int char_len_pointer = symbols_cnt - 1;
    while (symbols_cnt > 0) {
        cnt_len_[index] = ReadNBitsNumber(BITS_ADVANCED_);
        for (int i = 0; i < cnt_len_[index]; i++) {
            char_len_[symbols_permutation[char_len_pointer--]] = index;
        }
        symbols_cnt -= cnt_len_[index++];
    }
    int non_tern_items[MAX_CHAR];
    std::fill(non_tern_items, non_tern_items + MAX_CHAR, 0);
    for (int i = MAX_CHAR - 1; i - 1 > 0; i--) {
        non_tern_items[i - 1] = ((non_tern_items[i] + cnt_len_[i]) >> 1);
    }
    for (int i = 0; i < MAX_CHAR; i++) {
        std::fill(compressed_bor_[i], compressed_bor_[i] + MAX_CHAR, -1);
    }
    for (auto ch : symbols_permutation) {
        code_map_[ch] = non_tern_items[char_len_[ch]]++;
        compressed_bor_[char_len_[ch]][code_map_[ch]] = ch;
    }
}

void Extract::Reset() {
    writing_block_pointer_ = 0;
}

void Extract::WriteInFile() {
    std::ofstream output(extracted_file_, std::ios::binary | std::ios::app);
    output.write(writing_block_, writing_block_pointer_);
    output.close();
}

void Extract::WriteChar(char ch) {
    writing_block_[writing_block_pointer_++] = ch;
    if (writing_block_pointer_ == BLOCK_SIZE) {
        WriteInFile();
        writing_block_pointer_ = 0;
    }
}

int Extract::DecodeChar() {
    int cur_len = 0;
    int cur_code = 0;
    while (compressed_bor_[cur_len][cur_code] == -1) {
        cur_code <<= 1;
        cur_code += (GetBit() ^ 1);
        cur_len++;
        if (cur_len >= MAX_CHAR || cur_code >= MAX_CHAR) {
            throw Exception::WrongFile();
        }
    }
    return compressed_bor_[cur_len][cur_code];
}

void Extract::ExtractAll() {
    if (!input_) {
        throw Exception::WrongFile();
    }
    int ch = 0;
    while (ch != ARCHIVE_END_) {
        Reset();
        BuildCodeMap();
        extracted_file_.clear();
        ch = DecodeChar();
        while (ch != FILENAME_END_) {
            extracted_file_.push_back(static_cast<unsigned char>(ch));
            ch = DecodeChar();
        }
        ClearFile(extracted_file_);
        ch = DecodeChar();
        while (ch != ONE_MORE_FILE_ && ch != ARCHIVE_END_) {
            WriteChar(static_cast<char>(ch));
            ch = DecodeChar();
        }
        WriteInFile();
    }
}
