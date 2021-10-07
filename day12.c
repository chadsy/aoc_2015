// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 12
//
// The one about summing all the numbers in a JSON document.
//
// Part 1 is to sum the value of every integer literal in the JSON blob. The
// good news is, we can ignore digits that appear inside strings; we're only
// looking for numbers as actual values in a k/v pair or an array. Meaning, we
// can basically ignore all the JSON punctuation (skipping over strings) and
// just sum the numbers (including '-' to allow for negative values).
//
// Part 2 is the oops about 'red' values. So we're supposed to ignore any object
// and it's children that have 'red' as a value. Now we have to actually try to
// pseudo-parse the JSON. The good news is, the JSON format is super-compressed,
// having no whitespace. Guess we can keep up the hackiness of the scanner and
// just manage to ignore anything inside an object def '{...}' that contains a
// string like ":"red" (including those quotes, that's the attribute magic).
//
// This was a booger, had several tries: 18487 is too low, 48241 is too low,
// 111023 is too high, 93046 is just wrong (no hi/low clue), but 68466 is just right.

#include <stdio.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

int read_integer(FILE *in) {
    char buf[30], *p = buf, c = fgetc(in);

    if (c == '-') {
        *p++ = c;
        c = fgetc(in);
    }

    while (isdigit(c) && c != EOF) {
        *p++ = c;
        c = fgetc(in);
    }
    *p = '\0';

    ungetc(c, in);
    return atoi(buf);
}

static int level = 0;

void eat_json_object(FILE *in) {
    // start with 1, because we got here from a '{'
    int depth = 1;
    char c;

    do {
        c = fgetc(in);
        if (c == '{')
            depth++;
        else if (c == '}')
            depth--;
    } while (depth != 0);
}

long sum_json_object_no_red(FILE *in) {
    long sum = 0;
    char c;
    bool seeking_quote = false;
    bool found_red = false;

    while ((c = fgetc(in)) != EOF) {
        if (c == '{') {
            if (found_red) {
                eat_json_object(in);
            }
            else {
                level++;
                long amt = sum_json_object_no_red(in);
                level--;
                sum += amt;
            }
        }
        else if (c == '}') {
            break;
        }
        else if (!seeking_quote && (c == '-' || isdigit(c))) {
            ungetc(c, in);
            int amt = read_integer(in);
            if (!found_red)
                sum += amt;
        }
        else if (c == '"') {
            seeking_quote = !seeking_quote;
        }
        else if (c == ':') {
            if ((c = fgetc(in)) == '"') {
                char *redstr = "red";
                char *r = redstr;
                seeking_quote = true;

                while ((c = fgetc(in)) != '"') {
                    if (c != *r)
                        break;

                    if ((r - redstr) == strlen(redstr) - 1) {
                        found_red = true;
                        sum = 0;
                        break;
                    }
                    r++;
                }
            }
            else
                ungetc(c, in);
        }
    }
    return sum;
}

long sum_simple_json(FILE *in) {
    long sum = 0;
    char c;
    bool seeking_quote = false;

    while ((c = fgetc(in)) != EOF) {
        if (!seeking_quote && (c == '-' || isdigit(c))) {
            ungetc(c, in);
            sum += read_integer(in);
        }
        else if (c == '"') {
            seeking_quote = !seeking_quote;
        }
    }

    return sum;
}

int main(int argc, char ** argv) {
//    FILE *args = fopen("inputs/day12.txt","rt");
    FILE *args = stdin;
    long sum = 0;

     sum = sum_simple_json(args);
     printf("Part 1: Sum of all integers in the JSON: %ld\n", sum);

     rewind(args);

    sum = sum_json_object_no_red(args);
    printf("Part 2: Sum of all integers in the JSON except 'red': %ld\n", sum);

    return 0;
}
