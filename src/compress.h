#pragma once
#include <memory>
#include <vector>
#include <iostream>
#include <map>
#include "basic.h"

class Compress : public Basic {
public:
    explicit Compress(char *result_file);
    void CompressAll(char **texts, int shift, int sz);

private:
    std::map<int, int64_t> frequency_;
    int max_len_;
    std::vector<int> symbols_permutation_;
    struct Bor {
        size_t dist = 0;
        std::shared_ptr<Bor> child[2] = {nullptr, nullptr};
        int64_t weight;
        bool is_tern = false;
        int ch;

        Bor(int64_t weight, int ch);
        Bor(std::shared_ptr<Bor> a, std::shared_ptr<Bor> b);
        ~Bor();
    };

    void Print(std::shared_ptr<Bor> cur, std::string &pth) const;
    void BuildKeys(std::shared_ptr<Bor> root);
    void BuildMap(char *file_name);
    std::shared_ptr<Bor> BuildBor() const;
    void CalcLen(std::shared_ptr<Bor> cur, int depth);
    void WriteCompressInfo(char *cur_file);
    void WriteNBitsNumber(int x, int n, int inv = 0);
    void AddBit(int x);
    void WriteChar();
    void WriteBlock() const;
    void EncodeAndWrite(int ch);
    int ReverseNBitsNumber(int x, int n) const;
    void ReadFile(char *file_name, void (Compress::*relax_char)(int));
    int ExtractFileFromRelativePath(char *cur_file) const;
    void UpdateMap(int ch);
    char *result_file_;
};
