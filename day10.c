// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 10
// The one about 'look-and-say' sequences, whatever the assrash those are.
//
// So it's like RLE, it seems. But the special sauce of this puzzle is to run
// the run-length encoding like 40 times. Then deliver to Santa the length of
// the string after the 40th run. I can't help but think that there's a set of
// answers that are technically wrong but have the right length. Like a hash
// collision, where only the insurance companies win. And apparently, this is
// an established mathmatical idiom.
//
// So...might have to make a couple of assumptions. Given the description of the
// puzzle, it looks like any run of digits is between between 1-9. Can't say how
// the description would work if there was, say, '1111111111112' as 12 ones and
// one two.
//
// Part 2 is same, except 50 iterations. It's amazing how long the damned thing
// grew on 40 iterations, guess I'll bump those static buffers again...

#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define ITERATIONS_1        40
#define ITERATIONS_2        50
#define MAX_STRING_LEN      0xffffff

char input[MAX_STRING_LEN];
char output[MAX_STRING_LEN];

void rle_encode(char *from, char *to) {
    char last = *from++;
    char run = 1;

    while (*from) {
        // in a run
        if (*from == last) {
            run++;
            from++;
        }
        // end one, start another run
        else {
            *to++ = run + '0';
            *to++ = last;
            last = *from++;
            run = 1;
        }
    }
    *to++ = run + '0';
    *to++ = last;
    *to = '\0';
}

int main(int argc, char **argv) {
    char *p = input;

    while ((*p = getchar()) != EOF)
        p++;
    *--p = '\0';

    char *orig = strdup(input);

    for (int i = 0; i < ITERATIONS_1; i++) {
        rle_encode(input, output);
        // printf("  %d: '%s'\n", i, output);
        if (i < ITERATIONS_1 - 1) {
            strcpy(input, output);
            output[0] = '\0';
        }
    }

    printf("Part 1, %d iterations, encoded to a string of length %lu\n", ITERATIONS_1, strlen(output));

    strcpy(input, orig);
    for (int i = 0; i < ITERATIONS_2; i++) {
        rle_encode(input, output);
        // printf("  %d: '%s'\n", i, output);
        if (i < ITERATIONS_2 - 1) {
            strcpy(input, output);
            output[0] = '\0';
        }
    }

    printf("Part 2, %d iterations, encoded to a string of length %lu\n", ITERATIONS_2, strlen(output));

    return 0;
}
