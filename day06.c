// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 6
//
// The one about the million lights in a 1000x1000 grid.
//
// So there are instructions specifying an operation and describing the opposite
// corners of a rectangle, expressed as:
//   [operation] [a,b] through [c,d]
//
// Hopefully, simple sscanf parsing can work. Then some basic functions to
// carry out the operation. Might be an 'operate' function pointer with a single
// implementation of rectangle traversal. Also need to think about efficiency
// of storing a million cells. I could write a bit array to get down to 125k,
// but who needs to write that? And the kind of bulk operations would be slow in
// most cases. So maybe just a big heap and some tasty pointer arithmetic on a
// collection of bools...
//
// Update: 525273 is not right, too low
// 570793 is not right, too high, but the code passes the puzzles test cases :(
//
// Part 2 is the same operations, same collection of lights. The difference is
// that instead of on or off, the operations increment and decrement each bulb's
// brightness. So refactoring everything to use ints instead of bools (2mb of
// memory for the collection!) and create new operation execution.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <sys/param.h>

char *trim(char *str) {
    char *p = str + (strlen(str) - 1);
    while (isspace(*p)) {
        *p-- = '\0';
    }
    return str;
}

#define WIDTH   1000
#define HEIGHT  1000

#define LIGHT_TYPE(x)   \
typedef x light; \
const char *light_type_name = #x

LIGHT_TYPE(double);

typedef enum {
    op_ON,
    op_OFF,
    op_TOGGLE,
    op_INVALID,
} OPCODE;

typedef struct {
    OPCODE op;
    int a, b;
    int c, d;
    char *thunk;
} operation;

operation parse_operation(const char *str) {
    operation o;

    if (strstr(str, "turn on ")) {
        o.thunk = "turn on ";
        o.op = op_ON;
    }
    else if (strstr(str, "turn off ")) {
        o.thunk = "turn off ";
        o.op = op_OFF;
    }
    else if (strstr(str, "toggle ")) {
        o.thunk = "toggle ";
        o.op = op_TOGGLE;
    }
    else {
        o.thunk = "(invalid)";
        o.op = op_INVALID;
    }

    char format[128];
    sprintf(format, "%s%%d,%%d through %%d,%%d", o.thunk);

    if (sscanf(str, format, &o.a, &o.b, &o.c, &o.d) != 4) {
        fprintf(stderr, "error: cannot parse '%s' with format '%s'.\n", str, format);
    }

    return o;
}

int count_lights_brightness(light *lights) {
    light *p = lights;
    int bright = 0;
    light *end = lights + (WIDTH * HEIGHT);

    while (p < end) {
        bright += *p++;
    }

    return bright;
}

int count_lights_on(light *lights) {
    light *p = lights;
    int count = 0;
    light *end = lights + (WIDTH * HEIGHT);

    while (p < end) {
        if (*p++)
            count++;
    }
    return count;
}

void apply_binary_op_to_row(OPCODE o, light *start, size_t count) {
    light *p = start;

    if (o == op_OFF)
        memset(start, 0, count * sizeof(light));
    else if (o == op_ON)
        while (p < start + count) {
            *p++ = 1;
        }
    else if (o == op_TOGGLE) {
        while (p < start + count) {
            *p = !*p;
            p++;
        }
    }
}

void apply_dimmable_op_to_row(OPCODE o, light *start, size_t count) {
    light *p = start;

    while (p < start + count) {
        if (o == op_ON)
            *p += 1;
        else if (o == op_OFF && *p > 0)
            *p -= 1;
        else if (o == op_TOGGLE)
            *p += 2;

        p++;
    }
}

void apply_operation(operation o, light *binary, light *dimmable) {
    int length = o.d - o.b + 1;

    for (int i = o.a; i <= o.c; i++ ) {
        int rowstart = i * WIDTH;
        apply_binary_op_to_row(o.op, binary + rowstart + o.b, length);
        apply_dimmable_op_to_row(o.op, dimmable + rowstart + o.b, length);
    }
}

void dump_operation(operation o) {
    printf("%s %d,%d through %d,%d\n", o.thunk, o.a, o.b, o.c, o.d);
}

int main(int argc, char **argv) {
    light *binary = calloc(WIDTH * HEIGHT, sizeof(light));
    if (!binary) {
        fprintf(stderr, "error: cannot allocate memory for the binary array.\n");
    }

    light *dimmable = calloc(WIDTH * HEIGHT, sizeof(light));
    if (!dimmable) {
        fprintf(stderr, "error: cannot allocate memory for the dimmable array.\n");
    }

    FILE *input = stdin;
    char arg[128];

    while (fgets(arg, sizeof(arg) - 1, input)) {
        trim(arg);
        operation o = parse_operation(arg);
        apply_operation(o, binary, dimmable);
    }

    printf("using lights of type '%s'\n", light_type_name);
    printf("Part 1, lights remaining on %d\n", count_lights_on(binary));
    printf("Part 2, total brightness %d\n", count_lights_brightness(dimmable));

    free(binary);
    free(dimmable);

    return 0;
}
