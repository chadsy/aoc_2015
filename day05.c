// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 5
// The one about applying rules to see which of the input strings are naughty
// or nice.
//
// So, there are rules about evaluating the string. There's a brute-force method
// to just check for each of the conditions (e.g. contains at least 3 unique
// vowels) on the string. That's a O(m*n*x) problem, roughly. There might be a way
// to optimize that down to just walking the string's characters and accumulating
// test results. At the start, no idea which I'll end up with.
//
// Edit: ended up using some the basic string library functions, like strchr()
// and strstr() and just tried to handle the logic with care.
//
// Fails: Part 2 is NOT 429

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <string.h>

char *trim(char *str) {
    char *p = str + (strlen(str) - 1);
    while (isspace(*p)) {
        *p-- = '\0';
    }
    return str;
}

bool contains_double_letter(const char *str) {
    const char *p = str + 1;
    char prev = *str;
    while (*p) {
        if (*p == prev) {
            return true;
        }
        prev = *p++;
    }

    return false;
}

bool contains_three_vowels(const char *str) {
    int found_count = 0;

    while (*str && found_count < 3) {
        if (strchr("aeiou", *str))
            found_count++;
        str++;
    }

    return found_count >= 3;
}

bool contains_blocklist(const char *str) {
    return (strstr(str, "ab") ||
            strstr(str, "cd") ||
            strstr(str, "pq") ||
            strstr(str, "xy"));
}

bool is_nice_first(const char *str) {
    if (contains_double_letter(str) &&
        contains_three_vowels(str) &&
        !contains_blocklist(str))
        return true;
    else
        return false;
}

bool contains_split_pair(const char *str) {
    if (strlen(str) < 3)
        return false;

    const char *p = str + 2;
    const char *prev = str;
    while (*p) {
        if (*p == *prev) {
            return true;
        }
        prev++, p++;
    }

    return false;
}

bool contains_non_overlapping_pair_pair(const char *str) {
    char pair[3];

    while (*str) {
        strncpy(pair, str, sizeof(pair) - 1);
        if (strstr(str + 2, pair))
            return true;
        str++;
    }

    return false;
}

bool is_nice_second(const char *str) {
    if (contains_split_pair(str) &&
        contains_non_overlapping_pair_pair(str))
        return true;
    else
        return false;
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    char arg[128];

    int first_nice_count = 0;
    int second_nice_count = 0;

    while (fgets(arg, sizeof(arg) - 1, input)) {
        trim(arg);

        if (is_nice_first(arg)) {
            first_nice_count++;
        }

        if (is_nice_second(arg)) {
            second_nice_count++;
        }
    }

    printf("part 1, easy rules: found %d nice strings\n", first_nice_count);
    printf("part 2, hard rules: found %d nice strings\n", second_nice_count);

    return 0;
}
