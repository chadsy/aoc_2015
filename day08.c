// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 8
//
// The one about counting the storage characters minus the actual characters
// after removing quotes and escapes.
//
// For part 1, it's simply removing whitespace (newlines), counting the string
// length, subracting 2 for the quotes at each end, then subtracting 2 for each
// '\' escape character. If the escape is followed by an 'x' then the next two
// characters are a hex character value, meaning subtract 2 more. We accumulate
// the count of raw characters and the count of net-characters, and subtract the
// two for the answer.
//
// I suspsect Part 2 will require something more smart than this, some actual
// parsing or something similar. Guess we'll see.
//
// Ok, Part 2 is...encode instead of decode the strings and sum that, then
// subtract the original amount. So it's a lot like 'fake' counting the decode.

#include <stdio.h>
#include <string.h>
#include <ctype.h>

char *trim(char *str) {
    char *p = str + (strlen(str) - 1);
    while (isspace(*p)) {
        *p-- = '\0';
    }
    return str;
}

// Part 1
int count_decoded_characters(char *str) {
    int len = strlen(str);

    // remove count of quotes at each end
    len -= 2;

    // remove one for each escape
    for (char *p = str + 1; (p = strchr(p, '\\')); p += 2) {
        len--;
        // if the escape is a \x then take off the two hex digits too
        if (*(p + 1) == 'x')
            len -= 2;
    }

    return len;
}

// Part 2
int count_encoded_characters(char *str) {
    int len = strlen(str);

    // add quotes at each end
    len += 2;

    for (char *p = str; *p; p++) {
        switch (*p) {
        case '"':
            len++;
            break;
        case '\\':
            len++;
            break;
        }
    }

    return len;
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    char arg[128];
    int raw_sum = 0;
    int net_sum = 0;
    int encoded_sum = 0;

    while (fgets(arg, sizeof(arg) - 1, input)) {
        trim(arg);
        int raw_count = strlen(arg);
        int net_count = count_decoded_characters(arg);
        int encoded_count = count_encoded_characters(arg);

        // printf("%-4d %-4d %-4d'%s'\n", raw_count, net_count, encoded_count, arg);
        raw_sum += raw_count;
        net_sum += net_count;
        encoded_sum += encoded_count;
    }

    printf("Part 1: %d raw characters, %d decoded characters, leaves a difference of %d\n",
        raw_sum, net_sum,
        raw_sum - net_sum);
    printf("Part 2: %d encoded characters, %d raw characters, leaves a difference of %d\n",
        encoded_sum, raw_sum,
        encoded_sum - raw_sum);

    return 0;
}