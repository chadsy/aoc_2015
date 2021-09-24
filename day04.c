// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 4
// The one about searching for MD5 hashes that start with '00000' for AdventCoins.
//
// It appears there's no built-in MD5 hash function at or near libc. So...will
// have to go find one and paste it in. In doing that, will do some additional
// research to understand the algorithm to the extent that some implementation
// insight might help me short-circuit the search. Otherwise, it's just start
// at 0 and generate a hash for each integer, then evaluate whether that MD5
// hash starts with '00000...' -- so worst case is O(n) but each operation could
// be expensive if the MD5 implementation sucks. Turns out, part 2 is same as
// part 1, but looking for 6 zeros, '000000', at the start of the hash.
//
// Update: openSSL has an MD5, should have expected that. That was an unexpected
// dependency. So, you gotta have openSSL installed.

#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <limits.h>
#include <openssl/md5.h>

char *trim(char *str) {
    char *p = str + (strlen(str) - 1);
    while (isspace(*p)) {
        *p-- = '\0';
    }
    return str;
}

unsigned char *calculate_MD5(unsigned char *input, size_t len) {
    static unsigned char hash[MD5_DIGEST_LENGTH];

    MD5(input, len, hash);

    return hash;
}

char *stringify_MD5(unsigned char *hash) {
    static char result[2 * MD5_DIGEST_LENGTH + 1];
    memset(result, 0, sizeof(result));

    for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
        sprintf(result + strlen(result), "%02x", hash[i]);
    }

    return result;
}

bool is_target_hash_5(const unsigned char *hash) {
    // since it's an array of bytes, the string representation of '00000' really
    // means that the first 2.5 bytes are 0; the third byte has 0 in the high
    // nibble, meaning less than 16.
    return *(hash + 0) == 0 &&
           *(hash + 1) == 0 &&
           *(hash + 2) < 16;
}

bool is_target_hash_6(const unsigned char *hash) {
    // since it's an array of bytes, the string representation of '000000'
    // can be tested by making sure that the first 3 bytes are 0.
    return *(hash + 0) == 0 &&
           *(hash + 1) == 0 &&
           *(hash + 2) == 0;
}

bool is_target_hash_str_5(const char *hash) {
    char target[] = "00000";
    return strncmp(hash, target, strlen(target)) == 0;
}

bool is_target_hash_str_6(const char *hash) {
    char target[] = "000000";
    return strncmp(hash, target, strlen(target)) == 0;
}

int main(int argc, char ** argv) {
    FILE    *args = stdin;
    char    secret[128];
    bool    found1 = false, found2 = false;
    int     seed;

    fgets(secret, sizeof(secret), args);
    trim(secret);

    clock_t start = clock();

    // Part 1, find the numeric value that, when appended to the input string,
    // results in a MD5 hash that begins with the sequence '00000'.
    // Part 2 is the same, except 6 zeros, '000000'. So we can easily check
    // both while looping and doing all the grunt work for hashes one time only.
    for (seed = 0; !(found2 && found1) && seed < INT_MAX; seed++) {
        char input[30];
        sprintf(input, "%s%d", secret, seed);

        unsigned char *hash = calculate_MD5((unsigned char *)input, strlen(input));
        char *hashstr = stringify_MD5(hash);

        if (!found1 && is_target_hash_str_5(hashstr)) {
            found1 = true;
            printf("part 1 (00000) : for '%s', the MD5 hash '%s' is made with %d\n", secret, hashstr, seed);
        }

        if (!found2 && is_target_hash_str_6(hashstr)) {
            found2 = true;
            printf("part 2 (000000): for '%s', the MD5 hash '%s' is made with %d\n", secret, hashstr, seed);
        }
    }

    clock_t end = clock();
    printf("Calculated hashes in %lf seconds, using string-ized hashes.\n", (double)(end - start) / CLOCKS_PER_SEC);

    // This implementation is using binary comparison for the first N bytes of
    // the hash, then rendering to string once a match is found.
    start = clock();
    found1 = found2 = false;

    for (seed = 0; !(found2 && found1) && seed < INT_MAX; seed++) {
        char input[30];
        sprintf(input, "%s%d", secret, seed);

        unsigned char *hash = calculate_MD5((unsigned char *)input, strlen(input));

        if (!found1 && is_target_hash_5(hash)) {
            found1 = true;
            printf("part 1 (00000) : for '%s', the MD5 hash '%s' is made with %d\n", secret, stringify_MD5(hash), seed);
        }

        if (!found2 && is_target_hash_6(hash)) {
            found2 = true;
            printf("part 2 (000000): for '%s', the MD5 hash '%s' is made with %d\n", secret, stringify_MD5(hash), seed);
        }
    }

    end = clock();
    printf("Calculated hashes in %lf seconds, using binary hashes.\n", (double)(end - start) / CLOCKS_PER_SEC);

    return 0;
}
