// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 3
// The one about Santa visiting houses on an infinite grid, based on simple
// up '^', down 'v', left '<', and right '>' instructions.
//
// Part 1 is that some locations on the grid (houses) are visited more than
// once, so we need to count only the number of unique visits, places that received
// 'at least one present'. Even though the instructions move Santa one house at a
// time, it's still possible to create a sparse grid. So...the grid is sort of like
// a 'snake' game, where you could see something like the perimeter of a square
// with an empty (unvisited) middle. Since the grid is infinite, it's
// representation will be a challenge.
//
// One insight is to ignore that the houses are in a grid, ovs, and merely make each
// one a sort of {x, y} tuple. Since we don't have to count the number of visits,
// the simplest solution is adding the tuple to a collection as its visited,
// and checking the collection before counting another visit (or allowing the
// collection code to ignore the duplicate.) This is perfect for a hashtable.
//
// So, find a hashtable, write a cheap one, or implement some other grid-like or
// linked-list-like data structure. Turns out, GCC contains 'search.h' and
// hsearch, etc. This is a simple singleton model, but that's perfect for what
// we want.
//
// Part 2 is just that there are two agents, taking alternating instructions.
// Counting and hash rules are the same. The problem is, I can only stream
// through the inputs once, and can only have one single of these lame singleton
// hsearch hash tables. I suppose I could add the agent's identity to the hash
// key, tripling the number of entries:
//    A1 (Santa prime, in the first part of the puzzle)
//    A2 (Santa duplicate, in the second part)
//    A3 (The robot, also in the second part of the puzzle)
// Then using 3 agent locations and 2 unique variations of the location hash
// key, I could determine unique counts (if X/Y/A1 is not hashed, increment the
// unique counter for part one; if X/Y/A2 is not hashed, increment
// the unique counter for part two).
//
// Closing note: ended up with printf() debugging so put them behind a debug
// switch and LOG() macro. Also, ended up using 'bool' so this may be a C99
// dependency as well.

#include <stdio.h>
#include <search.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

#define DEBUG 0

#if DEBUG
#define LOG(...)       printf(__VA_ARGS__)
#else
#define LOG(...)
#endif

typedef struct {
    int x;
    int y;
    char *id;
} AGENT;

char *create_key(AGENT agent) {
    char buf[40];
    memset(buf, 0, sizeof(buf));
    snprintf(buf, sizeof(buf), "%s@(%d,%d)", agent.id, agent.x, agent.y);
    return strdup(buf);
}

bool has_visited(AGENT agent) {
    ENTRY searching;
    searching.key = create_key(agent);
    searching.data = NULL;
    ENTRY *found = hsearch(searching, FIND);
    LOG("[%s] has %svisited\n", searching.key, (found == NULL) ? "NOT " : "");
    free(searching.key);
    return found != NULL;
}

int main(int argc, char ** argv) {
    int santa_unique = 1;     // count the starting location as one house
    int team_unique = 1;
    FILE *input = stdin;
    char c = 0;

    if (hcreate(2000) == 0) {
        fprintf(stderr, "error: cannot allocate hashtable.\n");
    }

    AGENT santa_prime = {0, 0, "prime"};
    AGENT santa_lazy = {0, 0, "team"};
    AGENT floyd = {0, 0, "team"};

    ENTRY entry = {create_key(santa_prime), NULL};
    if (!hsearch(entry, ENTER))
        LOG("add error for %s\n", entry.key);
    has_visited(santa_prime);

    entry.key = create_key(santa_lazy);
    if (!hsearch(entry, ENTER))
        LOG("add error for %s\n", entry.key);
    has_visited(santa_lazy);

    entry.key = create_key(floyd);
    if (!hsearch(entry, ENTER))
        LOG("add error for %s\n", entry.key);
    has_visited(floyd);

    AGENT *alternates[] = {
        &santa_lazy,
        &floyd
    };

    int alternate_turn = 0;

    while ((c = fgetc(input)) != EOF) {
        LOG("counts: prime %d, team %d\n", santa_unique, team_unique);
        AGENT *team_current = alternates[alternate_turn++ % 2];
        bool process = true;
        switch (c) {
        case 'v':
            santa_prime.y++;
            team_current->y++;
            break;
        case '^':
            santa_prime.y--;
            team_current->y--;
            break;
        case '>':
            santa_prime.x++;
            team_current->x++;
            break;
        case '<':
            santa_prime.x--;
            team_current->x--;
            break;
        default:
            // ignore
            process = false;
            break;
        }

        if (process) {
            LOG("evaluating [%c, 0x%x] for santa_prime, %s\n", c, c, team_current->id);

            if (!has_visited(santa_prime)) {
                entry.key = create_key(santa_prime);
                if (!hsearch(entry, ENTER))
                    LOG("add error for %s\n", entry.key);
                santa_unique++;
            }

            bool sl = has_visited(santa_lazy);
            bool fl = has_visited(floyd);

            if (!sl || !fl) {
                entry.key = create_key(*team_current);
                if (!hsearch(entry, ENTER))
                    LOG("add error for %s\n", entry.key);
                team_unique++;
            }
        }
    }

    hdestroy();

    printf("part 1: houses visited at least once: %d\n", santa_unique);
    printf("part 2: houses visited at least once with two agents: %d\n", team_unique);

    return 0;
}
