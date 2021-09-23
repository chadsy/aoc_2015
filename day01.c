// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 1
// The one about Santa riding an elevator based on instructions he doesn't understand.
//
// It's actually simply stated as '(' goes up one floor and ')' means go down.
// Nothing complicated about matching pairs and shit, and for part two, counting
// along with how many are evaluated by the first time he hits the basement (-1).

#include <stdio.h>

int main(int argc, char **argv) {
    FILE *input = stdin;

    int floor = 0;
    int position = 0;
    int evals = 0;
    char c;

    while ((c = fgetc(input)) != EOF) {
        if (c == '(') {
            evals++;
            floor++;
        }
        else if (c == ')') {
            evals++;
            floor--;
        }
        if (floor == -1 && position == 0) {
            position = evals;
        }
    }

    printf("part one: floor %d\n", floor);
    printf("part two: position %d\n", position);

    return 0;
}