#include "exception.h"

const std::tuple<std::string, std::string, std::string> Exception::FEW_ARGS = {
    "Too few arguments. The program needs at least ", ", but only ", " given"};
const std::string Exception::BASIC_FORMAT_ERROR = "Incorrect command format";
const std::string Exception::CANT_READ_FILE = "Can't read file";
const std::string Exception::SAME_ARGS = "Two or more arguments are the same.";
const std::string Exception::WRONG_FILE = "The file is corrupted or doesn't exist.";

Exception::Exception(const std::string& error_message) : erorr_message_(error_message) {
}

const char* Exception::what() const noexcept {
    return erorr_message_.c_str();
}

Exception Exception::FewArgs(int needed, int given) {
    return Exception(std::get<0>(Exception::FEW_ARGS) + std::to_string(needed) + std::get<1>(Exception::FEW_ARGS) +
                     std::to_string(given) + std::get<2>(Exception::FEW_ARGS));
}

Exception Exception::BasicFormatError() {
    return Exception(Exception::BASIC_FORMAT_ERROR);
}

Exception Exception::CantReadFile() {
    return Exception(Exception::CANT_READ_FILE);
}

Exception Exception::SameArgs() {
    return Exception(Exception::SAME_ARGS);
}

Exception Exception::WrongFile() {
    return Exception(Exception::WRONG_FILE);
}
