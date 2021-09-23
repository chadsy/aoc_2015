// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 2
// The one about the elves needing a wrapping paper order that's based on the
// dimensions of the waiting packages.
//
// Will read L x W x H from stdin as "2x3x4" with optional newline, calculate
// total area of all 6 sides, and also add in extra slack. Slack amount is the
// same as the smallest side. One insight is that which of the dimensions is
// L or W or H doesn't matter, the calculation is the same. All dimensions are
// in feet (meaning, no conversions) and all are integers, there are no
// division operations.

#include <stdio.h>

int smallest(int a, int b, int c) {
    if (a <= b && a <= c)
        return a;
    else if (b <= a && b <= c)
        return b;
    else if (c <= a && c <= b)
        return c;
    else
        return 0;
}

int largest(int a, int b, int c) {
    if (a >= b && a >= c)
        return a;
    else if (b >= a && b >= c)
        return b;
    else if (c >= a && c >= b)
        return c;
    else
        return 0;
}

int calculate_paper(char * dims) {
    int L, W, H;
    int slop;

    sscanf(dims, "%dx%dx%d", &L, &W, &H);

    int x = L * W;
    int y = L * H;
    int z = H * W;
    slop = smallest(x, y, z);
    return x + x + y + y + z + z + slop;
}

int calculate_ribbon(char * dims) {
    int L, W, H;
    int bow;
    int ribbon;

    sscanf(dims, "%dx%dx%d", &L, &W, &H);
    ribbon = L + W + H - largest(L, W, H);
    return ribbon + ribbon + (L * W * H);
}

int main(int argc, char ** argv) {
    FILE    *input = stdin;
    char    buf[30];
    long    total_paper = 0;
    long    total_ribbon = 0;

    while (fgets(buf, sizeof(buf), input)) {
        total_paper += calculate_paper(buf);
        total_ribbon += calculate_ribbon(buf);
    }

    printf("total paper: %ld\n", total_paper);
    printf("total ribbon: %ld\n", total_ribbon);
}