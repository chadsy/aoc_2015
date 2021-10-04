// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 11
// The one about 'incrementing' Santa's password until all the rules work.
//
// Part 1 is just figure out how to increment Santa's current password according
// to, basically, base-26 math. At each increment, test some rules. The question
// is, do I do string processing, or use radix-foo to convert the base-26 password
// to a long, increment and back-convert/test each iteration? The conversion
// got nasty so I just went with a simple character-based increment. I did set
// up an optimization, checking for the disallowed characters during the
// increment operation so that I didn't have to check later in the validation
// process. Doing so yielded a more than 2x speedup in the 'production' run.
//
// Part 2 is running it again to find the next after the previous next.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>
#include <time.h>

#define OPTIMIZE    1

char *trim(char *str) {
    char *p = str + (strlen(str) - 1);
    while (isspace(*p)) {
        *p-- = '\0';
    }
    return str;
}

#define PASSWORD_RADIX      26
#define PASSWORD_LENGTH     8

char *increment_pwd_string(char *pwd) {
    for (int i = strlen(pwd) - 1; i >= 0; i--) {
        pwd[i]++;

#if OPTIMIZE
        if (pwd[i] == 'i' || pwd[i] == 'o' || pwd[i] == 'l')
            pwd[i]++;
#endif

        if ((pwd[i] - 'a') % PASSWORD_RADIX)
            return pwd;

        pwd[i] = 'a';
    }

    return pwd;
}

bool pwd_has_sequence(char *pwd) {
    for (int i = 2; i < strlen(pwd); i++) {
        if ((pwd[i-2] == pwd[i-1] - 1) &&
            (pwd[i-1] == pwd[i] - 1))
            return true;
    }
    return false;
}

bool pwd_has_pairs(char *pwd) {
    int count = 0;

    for (int i = 1; i < strlen(pwd) && count < 2; i++) {
        if (pwd[i-1] == pwd[i])
            count++, i++;
    }

    return count == 2;
}

bool is_pwd_valid(char *pwd) {
    if (strcmp(pwd, "abcdffaa") == 0)
        printf("tracing...\n");

    // check length
    if (strlen(pwd) != PASSWORD_LENGTH)
        return false;

    char *p;

    // check is lowercase
    for (p = pwd; islower(*p); p++)
        ;
    if (*p)
        return false;

#if !OPTIMIZE
    // check for disallowed characters if we didn't already skip
    for (p = pwd; !strchr("iol", *p); p++)
        ;
    if (*p)
        return false;
#endif

    // check the sequence and pairs requirements
    if (pwd_has_sequence(pwd) && pwd_has_pairs(pwd))
        return true;

    return false;
}

int main(int argc, char ** argv) {
    FILE    *args = stdin;
    char    old_pwd[30];
    char    new_pwd[30];
    int     iterations = 0;

    fgets(old_pwd, sizeof(old_pwd), args);
    trim(old_pwd);

    clock_t start = clock();

    strcpy(new_pwd, old_pwd);
    do {
        increment_pwd_string(new_pwd);
        iterations++;
    } while (!is_pwd_valid(new_pwd));

    clock_t end = clock();

    printf("Part 1: old pwd '%s', increments %d times over %lf secs to new pwd '%s'\n",
        old_pwd, iterations,
        (double)(end - start) / CLOCKS_PER_SEC,
        new_pwd);

    start = clock();

    // part 2, using the previous new pwd as the old and run it again
    strcpy(old_pwd, new_pwd);
    do {
        increment_pwd_string(new_pwd);
        iterations++;
    } while (!is_pwd_valid(new_pwd));

    end = clock();

    printf("Part 2: old pwd '%s', increments %d times over %lf secs to new pwd '%s'\n",
        old_pwd, iterations,
        (double)(end - start) / CLOCKS_PER_SEC,
        new_pwd);

    // long old_pwd_val = to_base_10(old_pwd);
    // printf("'%s' from base 26 to base 10: %ld\n", old_pwd, old_pwd_val);

    // printf("%ld from base 10 to base 26: '%s'\n", old_pwd_val, to_base_26_string(old_pwd_val));

    // long old_pwd_val = to_base_10(old_pwd);
    // printf("'%s' from base 26 to base 10: %ld\n", old_pwd, old_pwd_val);

    // long new_pwd_val = old_pwd_val + 1;


    // printf("Part 1: old pwd '%s', %d increments to new pwd '%s'", old_pwd, iterations, new_pwd);

    return 0;
}