#include <fstream>
#include <string>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <queue>
#include <functional>
#include "compress.h"
#include "exception.h"
#include "priority_queue.h"

Compress::Compress(char *result_file) : result_file_(result_file) {
}

void Compress::CompressAll(char **texts, int shift, int sz) {
    ClearFile(result_file_);
    for (int i = shift; i < sz; i++) {
        BuildMap(texts[i]);
        BuildKeys(BuildBor());
        WriteCompressInfo(texts[i]);
        if (i != sz - 1) {
            EncodeAndWrite(ONE_MORE_FILE_);
        } else {
            EncodeAndWrite(ARCHIVE_END_);
        }
    }
    if (char_pointer_ != 0) {
        WriteChar();
    }
    WriteBlock();
}

void Compress::WriteBlock() const {
    std::ofstream output(result_file_, std::ios::binary | std::ios::app);
    output.write(writing_block_, block_pointer_);
    output.close();
}

void Compress::WriteChar() {
    writing_block_[block_pointer_++] = static_cast<char>(char_buffer_);
    if (block_pointer_ == BLOCK_SIZE) {
        WriteBlock();
        block_pointer_ = 0;
    }
}

void Compress::AddBit(int bt) {
    char_buffer_ += (bt << (BITS_ - 1 - char_pointer_++));
    if (char_pointer_ == BITS_) {
        WriteChar();
        char_pointer_ = 0;
        char_buffer_ = 0;
    }
}

void Compress::WriteNBitsNumber(int x, int n, int inv) {
    for (int i = n - 1; i >= 0; i--) {
        AddBit(((x >> i) & 1) ^ inv);
    }
}

int Compress::ReverseNBitsNumber(int x, int n) {
    int res = 0;
    for (int i = 0; i < n; i++) {
        res += ((x & 1) << (n - i - 1));
        x >>= 1;
    }
    return res;
}

void Compress::EncodeAndWrite(int ch) {
    WriteNBitsNumber(code_map_[ch], char_len_[ch], 1);
}

void Compress::ReadFile(char *file_name, void (Compress::*relax_char)(int)) {
    std::ifstream input(file_name, std::ios::in | std::ios::binary);
    if (!input) {
        throw Exception::CantReadFile();
    }
    while (true) {
        input.read(reading_block_, BLOCK_SIZE);
        for (int i = 0; i < input.gcount(); i++) {
            (this->*relax_char)(static_cast<unsigned char>(reading_block_[i]));
        }
        if (!input) {
            break;
        }
    }
    input.close();
}

int Compress::ExtractFileFromRelativePath(char *cur_file) {
    int pos = -1;
    for (int i = 0; cur_file[i] != 0; i++) {
        if (cur_file[i] == '/') {
            pos = i;
        }
    }
    return pos;
}

void Compress::WriteCompressInfo(char *cur_file) {
    WriteNBitsNumber(frequency_.size(), BITS_ADVANCED_);
    for (auto it = symbols_permutation_.rbegin(); it != symbols_permutation_.rend(); it++) {
        WriteNBitsNumber(*it, BITS_ADVANCED_);
    }
    for (int i = 1; i <= max_len_; i++) {
        WriteNBitsNumber(cnt_len_[i], BITS_ADVANCED_);
    }

    for (int i = ExtractFileFromRelativePath(cur_file) + 1; cur_file[i] != 0; i++) {
        EncodeAndWrite(static_cast<unsigned char>(cur_file[i]));
    }
    EncodeAndWrite(FILENAME_END_);

    ReadFile(cur_file, &Compress::EncodeAndWrite);
}

void Compress::UpdateMap(int ch) {
    frequency_[ch]++;
}

void Compress::BuildMap(char *file_name) {
    frequency_.clear();
    ReadFile(file_name, &Compress::UpdateMap);
    for (int i = ExtractFileFromRelativePath(file_name) + 1; file_name[i] != 0; i++) {
        frequency_[file_name[i]]++;
    }
    frequency_[FILENAME_END_] = 1;
    frequency_[ONE_MORE_FILE_] = 1;
    frequency_[ARCHIVE_END_] = 1;
}

std::shared_ptr<Compress::Bor> Compress::BuildBor() const {
    auto cmp = [](const std::shared_ptr<Bor> &a, const std::shared_ptr<Bor> &b) {
        return std::make_pair(a->weight, a->ch) < std::make_pair(b->weight, b->ch);
    };
    PriorityQueue<std::shared_ptr<Bor>, decltype(cmp)> nodes;
    for (auto [ch, weight] : frequency_) {
        nodes.Push(std::make_shared<Bor>(Bor(weight, ch)));
    }
    while (nodes.Size() > 1) {
        std::shared_ptr<Bor> a = nodes.Top();
        nodes.Pop();
        std::shared_ptr<Bor> b = nodes.Top();
        nodes.Pop();
        nodes.Push(std::make_shared<Bor>(Bor(std::move(a), std::move(b))));
    }
    std::string pth;
    Print(nodes.Top(), pth);
    return nodes.Top();
}

void Compress::CalcLen(const std::shared_ptr<Bor> &cur, int depth) {
    if (!cur) {
        return;
    }
    max_len_ = std::max(max_len_, depth);
    if (cur->is_tern) {
        char_len_[cur->ch] = depth;
        cnt_len_[depth]++;
    }
    CalcLen(cur->child[0], depth + 1);
    CalcLen(cur->child[1], depth + 1);
}

void Compress::BuildKeys(const std::shared_ptr<Bor> &root) {
    std::fill(cnt_len_, cnt_len_ + MAX_CHAR, 0);
    max_len_ = 0;
    CalcLen(root, 0);
    symbols_permutation_.clear();
    for (auto [ch, weight] : frequency_) {
        symbols_permutation_.push_back(ch);
    }
    sort(symbols_permutation_.begin(), symbols_permutation_.end(),
         [&](int a, int b) { return std::tie(char_len_[a], a) > std::tie(char_len_[b], b); });
    int non_tern_items[max_len_ + 1];
    std::fill(non_tern_items, non_tern_items + max_len_ + 1, 0);
    for (int i = max_len_; i - 1 > 0; i--) {
        non_tern_items[i - 1] = ((non_tern_items[i] + cnt_len_[i]) >> 1);
    }
    for (auto ch : symbols_permutation_) {
        code_map_[ch] = non_tern_items[char_len_[ch]]++;
    }
}

Compress::Bor::Bor(int64_t weight, int ch) : weight(weight), is_tern(true), ch(ch) {
}

Compress::Bor::Bor(std::shared_ptr<Bor> &&a, std::shared_ptr<Bor> &&b)
    : weight(a->weight + b->weight), ch(std::min(a->ch, b->ch)) {
    child[0] = a;
    child[1] = b;
}
Compress::Bor::~Bor() {
}

void Compress::Print(const std::shared_ptr<Bor> &cur, std::string &pth) const {
    if (!cur) {
        return;
    }
    pth.push_back('0');
    Print(cur->child[0], pth);
    pth.pop_back();

    pth.push_back('1');
    Print(cur->child[1], pth);
    pth.pop_back();
}
