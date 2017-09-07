You are given a binary file containing unsorted unsigned 32-bit integers.
The file contains up to 2^20 random, unique integers, encoded in big-endian format.
Each integer will be in the range [0, 2^25).

Write a program that, given an integer in range [0, 2^25), will return the closest integer from the input file.
If there is a tie for the closest, return the lower one.

Your program should take one command-line argument:

./query <path_to_input_file>

Your program should read test cases from standard input and write results to standard output, one per line.

Constraints
------------------------------

1. Must be written in C or C++, using only standard libraries
    * Will compile with: g++ <sourcefile> -o query
    * Will run with: ./query <inputfile>.bin
2. Only 35 KB of system RAM available, but infinite disk space (can create new files as needed)
    * Simulates a small embedded system with external hard disk
    * Will be checking static mem usage with objdump (objdump -x query | grep bss | head -1)
    * Will be checking dynamic mem usage with valgrind
    * mmap cannot be used
3. Try to minimize file I/O and dynamic memory usage
4. Optimize for fast queries

Test Case Input/Output format
------------------------------

First line of input contains an integer, N (1 <= N <= 10000), denoting the number of test cases, followed by N query integers, one per line.
For each of the N query integers, output the closest integer from the input file, one per line.

Sample Input File (hexadecimal, easy_input.bin)
------------------------------

00 00 00 80 00 00 00 61 00 00 00 60 00 00 00 2E 00 00 00 0B 00 00 00 02 00 00 00 06 00 00 00 7F

Explanation:

Binary file containing 8 random, unique, unsorted, 32-bit unsigned integers: [128, 97, 96, 46, 11, 2, 6, 127]

Sample Test Cases
-----------------
5
0
255
90
4
16


Sample Test Cases Output
-----------------
2
128
96
2
11

Explanation:

N = 5, the number of test cases
Closest to 0 is 2
Closest to 255 is 128
Closest to 90 is 96
Closest to 4 is either 2 or 6, break tie by choosing lower (2)
Closest to 16 is 11

Additional Notes
-----------------
* Your program will be tested against hard_input.bin (contains 2^20 integers). Test cases will be hidden.
* You can test your program on easy_input.bin with:
    ./query easy_input.bin <easy_stdin.txt >easy_stdout.txt
    diff easy_stdout.txt easy_expected_stdout.txt
