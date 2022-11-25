#include <vector>
#include <iostream>
#include <string>
#include <string.h>
#include <set>
#include "exception.h"
#include "compress.h"
#include "extract.h"

const std::string HELP_MESSAGE =
    "archiver -c archive_name file1 [file2 ...] - compress files file1, file2, ... and save the result to the "
    "archive_name file.\n"
    "archiver -d archive_name - extract files from the archive_name archive and put them in the current directory.\n"
    "archiver -h - output help on using the program.";

bool SameArgs(int argc, char** argv) {
    std::set<std::string_view> unique_args;
    for (int i = 0; i < argc; i++) {
        unique_args.insert(argv[i]);
    }
    return argc != static_cast<int>(unique_args.size());
}

void Archiver(int argc, char** argv) {
    if (argc < 2 || (strcmp(argv[1], "-c") && strcmp(argv[1], "-d") && strcmp(argv[1], "-h"))) {
        throw Exception::BasicFormatError();
    } else if (SameArgs(argc, argv)) {
        throw Exception::SameArgs();
    } else if (!strcmp(argv[1], "-c")) {
        if (argc < 4) {
            throw Exception::FewArgs(4, argc);
        } else {
            Compress compress(argv[2]);
            compress.CompressAll(argv, 3, argc);
        }
    } else if (!strcmp(argv[1], "-d")) {
        if (argc != 3) {
            throw Exception::BasicFormatError();
        } else {
            Extract extract(argv[2]);
            extract.ExtractAll();
        }
    } else {
        if (argc != 2) {
            throw Exception::BasicFormatError();
        } else {
            std::cout << HELP_MESSAGE << "\n";
        }
    }
}

int main(int argc, char** argv) {
    try {
        Archiver(argc, argv);
    } catch (const std::exception& e) {
        std::cerr << e.what() << "\n";
        return Exception::CODE_ERROR;
    }
    return 0;
}
