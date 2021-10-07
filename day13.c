// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 13
// The one about finding a seating arrangement that optimizes people's happiness.
//
// This seems like another traveling salesperson problem, except that it's circular
// so there's an extra calculation to go from end back to start (well, to calc
// the happiness of the last person sitting next to the first) and also requires
// us to calcuate the happiness factor in both directions.
//
// Much of the code was, in fact, copied from day09. And yet, even with the needed
// algorithmic changes to account for a happiness value in both directions and
// looping around, it's a struggle. Once running with lots of printf'ing, 546 is
// not the right answer, too low. Once I get this working, I can exploit the
// insight that there aren't n! permutations and try to replicate. As a circle,
// the permutations are actually more like (n-1)! because the variations can
// start and end anywhere, and also that they can go in either direction. Working
// now, answer is 664.
//
// Part 2 is to stick me in the list, with 0 in all directions. Logically, it
// seems like the solution should be to stick me between the lowest net-happiness
// pairing. So...the answer would be 664 - 24 (frank <-> carol), or 640. That
// turns out to be the right answer. So if trying to optimize, I could just find
// the lowest pairing and stick myself in there.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define MAX_EATERS      10
#define MAX_NAME_LENGTH 20

typedef char hash_t;

int eater_count = 0;
char *all_eaters[MAX_EATERS];

#define eater_from_hash(h)   (all_eaters[h % eater_count])

hash_t hash_from_eater(char *eater) {
    for (int i = 0; i < eater_count; i++) {
        if (strcmp(eater, all_eaters[i]) == 0)
            return (hash_t)i;
    }
    return -1;
}

hash_t add_eater(char *eater) {
    hash_t hash = 0;

    if ((hash = hash_from_eater(eater)) == -1) {
        hash = eater_count;
        all_eaters[eater_count++] = strdup(eater);
    }

    return hash;
}

int happiness[MAX_EATERS * MAX_EATERS];

#define get_happiness(from, to)      (*(happiness + from + (to * MAX_EATERS)))
#define set_happiness(from, to, h)   *(happiness + from + (to * MAX_EATERS)) = h

void parse_happiness(char *str) {
    char from[MAX_NAME_LENGTH], to[MAX_NAME_LENGTH];
    int happy = 0;

    if (sscanf(str, "%s would lose %d happiness units by sitting next to %[^.].\n", from, &happy, to) == 3) {
        happy = -happy;
    }
    else if (sscanf(str, "%s would gain %d happiness units by sitting next to %[^.].\n", from, &happy, to) != 3) {
        fprintf(stderr, "error: cannot parse input string: %s\n", str);
    }

    hash_t f = add_eater(from);
    hash_t t = add_eater(to);

    set_happiness(f, t, happy);
}

hash_t final_seating[MAX_EATERS];
int max_happiness = 0;

#define swap(a, b)      { hash_t tmp; tmp = a; a = b; b = tmp; }

#define net_happiness(from, to)     ((get_happiness(from, to))+(get_happiness(to, from)))

int calculate_happiness(hash_t *eaters, int end) {
    int happy = 0;
    for (int i = 0; i < end; i++) {
        happy += net_happiness(eaters[i], eaters[i + 1]);
    }

    happy += net_happiness(eaters[end], eaters[0]);

    return happy;
}

void list_eaters(hash_t *eaters, int max) {
    printf("  ");
    for (int i = 0; i < max; i++) {
        printf("%s, ", eater_from_hash(eaters[i]));
    }
    printf("%s\n", eater_from_hash(eaters[max]));
}

int evaluations = 0;

void explore_journey(hash_t *eaters, int start, int end) {
    if (start == end) {
        evaluations++;
        int happy = calculate_happiness(eaters, end);

        // printf("found the end, %d: ", happy);
        // list_eaters(eaters, end);

        if (happy > max_happiness) {
            max_happiness = happy;
            memcpy(final_seating, eaters, (end + 1) * sizeof(hash_t));
        }
    }
    else {
        for (int i = start; i <= end; i++) {
            swap(eaters[i], eaters[start]);
            explore_journey(eaters, start + 1, end);
            swap(eaters[i], eaters[start]);
        }
    }
}

void dump_eaters() {
    for (int e = 0; e < eater_count; e++) {
        printf("hash %d eater %s\n", e, eater_from_hash(e));
    }
}

void dump_happiness_table() {
    printf("          : ");
    for (int eater = 0; eater < eater_count; eater++)
        printf("%-7s : ", eater_from_hash(eater));
    printf("\n");
    for (int r = 0; r < eater_count; r++) {
        printf(": %7s : ", eater_from_hash(r));
        for (int c = 0; c < eater_count; c++) {
            char hap[20];
            int h = get_happiness(r,c);
            sprintf(hap, "%3d", h);
            printf("%7s : ", (h != 0) ? hap : "---");
        }
        printf("\n");
    }
}

void dump_seating() {
    for (int eater = 0; eater < eater_count; eater++) {
        printf("%s\n", eater_from_hash(final_seating[eater]));
        printf(" ⬇️  %3d\n ------ %3d\n ⬆️  %3d\n",
            get_happiness(final_seating[eater], final_seating[(eater + 1)%eater_count]),
            net_happiness(final_seating[eater], final_seating[(eater + 1)%eater_count]),
            get_happiness(final_seating[(eater + 1)%eater_count], final_seating[eater])
            );
    }
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    char arg[128];

    while (fgets(arg, sizeof(arg) - 1, input)) {
        parse_happiness(arg);
    }

    // dump_eaters();
    // dump_happiness_table();

    hash_t *working_seats = calloc(eater_count, sizeof(hash_t));
    for (int i = 0; i < eater_count; i++)
        working_seats[i] = i;

    explore_journey(working_seats, 0, eater_count - 1);

    // dump_seating();

    printf("Part 1: %d evaluations, greatest happiness %d:\n", evaluations, max_happiness);
    list_eaters(final_seating, eater_count - 1);

    // add 'me' to eaters and happiness
    hash_t me = add_eater("Warren");
    for (int i = 0; i < eater_count - 1; i++) {
        set_happiness(i, me, 0);
        set_happiness(me, i, 0);
    }

    // dump_eaters();
    // dump_happiness_table();

    max_happiness = 0;

    working_seats = calloc(eater_count, sizeof(hash_t));
    for (int i = 0; i < eater_count; i++)
        working_seats[i] = i;

    explore_journey(working_seats, 0, eater_count - 1);

    // dump_seating();

    printf("Part 2: %d evaluations, greatest happiness %d:\n", evaluations, max_happiness);
    list_eaters(final_seating, eater_count - 1);

    return 0;
}
