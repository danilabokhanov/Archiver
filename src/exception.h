#pragma once
#include <iostream>
#include <string_view>
#include <tuple>

class Exception : public std::exception {
public:
    explicit Exception(const std::string& erorr_message);
    const char* what() const noexcept override;
    static Exception FewArgs(int needed, int given);
    static Exception BasicFormatError();
    static Exception CantReadFile();
    static Exception SameArgs();
    static Exception WrongFile();
    static const int CODE_ERROR = 111;

private:
    static const std::tuple<std::string, std::string, std::string> FEW_ARGS;
    static const std::string BASIC_FORMAT_ERROR;
    static const std::string CANT_READ_FILE;
    static const std::string SAME_ARGS;
    static const std::string WRONG_FILE;
    const std::string erorr_message_;
};
