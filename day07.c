// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 7
//
// The one about Little Bobby Tables getting a cheap knockoff of a verilog kit.
//
// Gah. This is the one that makes me wish I had chosen something like Python
// or super modern C++ for implementation. My initial guess is that the basic
// structure is a DAG of sorts, with the instructions specifying nodes and
// relationships. Thinking a bit more, it might be that the nodes can be
// represented as a simple dictionary, and execute by recursion on the affects
// of each operation.
//
// So the main observation after looking harder is that it really is a graph but
// it's not expressed in a useful form that indicates a graph. Example:
//     123 -> x             ; x = 123
//     456 -> y             ; y = 456
//     789 -> b             ; b = 789
//     x AND y -> z         ; z = x AND y
//     z OR b -> c          ; c = z OR b
//     c LSHIFT 2 -> a      ; a = c LSHIFT 2
// If we want (a), we'll need to recurse the operands:
//     a = (( (x)  AND  (y))  OR  (b))  LSHIFT 2
// or
//     a = (((123) AND (456)) OR (789)) LSHIFT 2
//
// The instructions are not delivered in any particular order, so will start by
// loading all the instructions (wires?) into a dictionary. Refactoring each
// instruction as an assignment expression. Will use lhs of the assignment (its
// destination) as the keys in the dictionary, and the values set to the
// expression on the rhs, stored as a string for later evaluation. Then we just
// start with our target value ('a' in the part 1) in the dictionary, parse its
// rhs expression, and walk its graph based on the operands. This will only walk
// the minimal set of evaluations.
//
// Screw it. Decided to just implement an array and do linear search. Profit.
// NB: Here's a tricky bit: the AND operator can have either a symbol or a constant
// (always '1' in the puzzle input) on its left-hand-side. Took a while to find
// THAT out the hard way, it's not in the instructions. Process note: if I cache
// a symbol's value (inside of 'evaluate_symbol_value') the first time it's
// evaluated, then use the cache, I get the right answer. If I don't, the damned
// thing (seems to) run forever. Hmmm...recursion fail?
//
// update: Another note missing from the instructions...there's an implied cache
// requirement. If I don't cache, the damned thing never finishes. I tried some
// simple code to detect a circular ref, but I never detected one. Nonetheless,
// without caching the results, I couldn't get this to work.
//
// Part 2 was simply resetting the instructions to not be cached, and replacing
// the expression for symbol 'b' with the value from part 1, then re-running.

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdbool.h>

char *trim(char *str) {
    char *p = str + (strlen(str) - 1);
    while (isspace(*p)) {
        *p-- = '\0';
    }
    return str;
}

int eval_count = 0;

typedef unsigned short signal_t;
typedef unsigned short hash_t;

typedef struct {
    hash_t id;
    char *sym;
    char *exp;
    bool evaluated;
    signal_t value;
} instruction;

// there are 339 in the input data, so...
#define MAX_SYMBOL_LENGTH   20
#define MAX_INSTRUCTIONS    500
int instruction_count = 0;
instruction instructions[MAX_INSTRUCTIONS];

// doing a dumb transliteration hash, that allows for up to 3 characters in a
// symbol name. it's not interesting except that it makes linear searching faster.
hash_t hash_symbol(char *sym) {
    int hash = 0;
    while (*sym) {
        hash = (hash << 5) + (*sym - 'a' + 1);
        sym++;
    }
    return hash;
}

instruction *find_instruction_for_symbol(char *sym) {
    for (int i = 0; i < instruction_count; i++) {
        // if (strcmp(sym, instructions[i].sym) == 0) {
        if (hash_symbol(sym) == instructions[i].id) {
            return &instructions[i];
        }
    }

    return NULL;
}

// need a forward declaration, not only for circular dependency but intermixed
// with recursion! i'm surprised the world hasn't ended
signal_t evaluate_symbol_value(char *sym);

signal_t evaluate_expression(char *exp) {
    signal_t result = 0;

    // Search for the binary operator keywords first
    // sscanf the parameters as 'sym1 operator sym2'
    if (strstr(exp, " AND ")) {
        char lhsym[MAX_SYMBOL_LENGTH], rhsym[MAX_SYMBOL_LENGTH];
        int lhconst = 0;

        // gotta handle a symbol or constant on lhs
        if (isalpha(*exp)) {
            sscanf(exp, "%s AND %s", lhsym, rhsym);
            result = evaluate_symbol_value(lhsym) & evaluate_symbol_value(rhsym);
        }
        else {
            sscanf(exp, "%d AND %s", &lhconst, rhsym);
            result = lhconst & evaluate_symbol_value(rhsym);
        }
    }
    else if (strstr(exp, " OR ")) {
        char lhsym[MAX_SYMBOL_LENGTH], rhsym[MAX_SYMBOL_LENGTH];
        sscanf(exp, "%s OR %s", lhsym, rhsym);
        result = evaluate_symbol_value(lhsym) | evaluate_symbol_value(rhsym);
    }
    // Search for the binary SHIFT operators, 'sym xSHIFT const'
    else if (strstr(exp, " LSHIFT ")) {
        char lhsym[MAX_SYMBOL_LENGTH];
        int val;
        sscanf(exp, "%s LSHIFT %d", lhsym, &val);
        result = evaluate_symbol_value(lhsym) << val;
    }
    else if (strstr(exp, " RSHIFT ")) {
        char lhsym[MAX_SYMBOL_LENGTH];
        int val;
        sscanf(exp, "%s RSHIFT %d", lhsym, &val);
        result = evaluate_symbol_value(lhsym) >> val;
    }
    // Search for the unary NOT operator, 'NOT sym'
    else if (strstr(exp, "NOT ")) {
        char rhsym[MAX_SYMBOL_LENGTH];
        sscanf(exp, "NOT %s", rhsym);
        result = ~evaluate_symbol_value(rhsym);
    }
    // Assume what's left is an immediate, so determine if its sym or const
    else {
        // It's a symbol
        if (islower(*exp)) {
            result = evaluate_symbol_value(exp);
        }
        // else it must be a constant
        else {
            unsigned literal;
            sscanf(exp, "%u", &literal);
            result = literal;
        }
    }

    return result;
}

signal_t evaluate_symbol_value(char *sym) {
    instruction *instr = find_instruction_for_symbol(sym);
    eval_count++;
    signal_t value = 0;

    if (!instr) {
        fprintf(stderr, "error: cannot find symbol '%s' in the instruction table.\n", sym);
    }
    else if (instr->evaluated) {
        // printf("cache hit for %s\n", sym);
        value = instr->value;
    }
    else {
        signal_t expr_value = evaluate_expression(instr->exp);
        if (instr->evaluated) {
            if (instr->value != expr_value) {
                fprintf(stderr, "error: expression for %s [%s] calculated as %u, cached as %u\n",
                    instr->sym, instr->exp, expr_value, instr->value
                    );
            }
        }

        instr->evaluated = true;
        instr->value = expr_value;
        value = expr_value;
    }

    return value;
}

char *crack_symbol(char *instruction) {
    char *p = strstr(instruction, " -> ");
    if (p) {
        p += 4;
        return strdup(p);
    }
    else
        fprintf(stderr, "error: unparseable instruction, %s\n", instruction);

    return NULL;
}

char *crack_expression(char *instruction) {
    char *exp = strdup(instruction);
    char *p = strstr(exp, " -> ");

    if (p) {
        *p = '\0';
        return exp;
    }
    else
        fprintf(stderr, "error: unparseable instruction, %s\n", instruction);

    free(exp);
    return NULL;
}

void add_instruction(char *sym, char *exp) {
    instruction instr;
    instr.id = hash_symbol(sym);
    instr.sym = strdup(sym);
    instr.exp = strdup(exp);
    instr.evaluated = false;
    instr.value = 0;
    instructions[instruction_count++] = instr;
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    char arg[128];

    while (fgets(arg, sizeof(arg) - 1, input)) {
        trim(arg);
        char *sym = crack_symbol(arg);
        char *exp = crack_expression(arg);
        add_instruction(sym, exp);
    }

    // part 1: just calc the value for symbol 'a'
    char *target_sym = "a";
    signal_t result = evaluate_symbol_value(target_sym);
    printf("Part 1: '%s' evaluates to %u\n", target_sym, result);

    // part 2:
    //     "Now, take the signal you got on wire a, override wire b to that
    //      signal, and reset the other wires (including wire a). What new
    //      signal is ultimately provided to wire a?"
    // so now 'a' has a value, clear all the instruction cached values, replace
    // the 'b' expression with a constant of that value, and run for 'a' again.
    for (int i = 0; i < instruction_count; i++) {
        instructions[i].evaluated = false;
        instructions[i].value = 0;
    }
    instruction *b_instr = find_instruction_for_symbol("b");
    char *prev = b_instr->exp;
    char b_expression[20];
    sprintf(b_expression, "%u", result);
    b_instr->exp = b_expression;
    // printf("...replacing %s's '%s' with '%s'\n", b_instr->sym, prev, b_instr->exp);
    result = evaluate_symbol_value(target_sym);
    printf("Part 2: '%s' by replacing 'b' with %s evaluates to %u\n", target_sym, b_instr->exp, result);

    return 0;
}
