# Archiver

This program compresses and extracts files using the [Huffman algorithm](https://en.wikipedia.org/wiki/Huffman_coding).

The following commands are available:
* `archiver -c archive_name file1 [file2 ...]` - compress files `file1, file2, ...` and save result to `archive_name`.
* `archiver -d archive_name` - extract files from `archive_name` and put them in the current directory.
* `archiver -h` - output help on using the program.

## Features

1. Block data reading is implemented. It is allows program to comfortably process files that are many times larger than RAM.
1. [Canonical Huffman codes](https://en.wikipedia.org/wiki/Canonical_Huffman_code) and optimizations are used to avoid explicit symbol search in the tree, reducing the work time.
1. Input errors are cathed. Helpful message is output.
1. The handwritten priority queue is available


## Test
Some tests examples in the "test" folder.
