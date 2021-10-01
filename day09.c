// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 9
//
// The one about plotting the shortest course to visit some new delivery locations.
//
// This appears to be a classic traveling salesperson problem...right down to
// "must visit each location exactly once." So back to the algorithm books I go...
// Just use a naive solution to find the distance of all the permutations, and
// then pick the shortest one.
//
// I started down one path, which we'll call "Cleverly make a first-order struct
// out of everything and implement the algorithm (mostly) like a collection of
// graphs." After a couple of seg faults that showed up as `Bus error: 10` I
// decided to reboot my thinking and use a table for the distances, starting
// points on one axis and finishes on the other. This was cleaner and simpler,
// obvs, with the caveat that there's no smarts on the algorithm. It's O(n!) for
// sure.
//
// Part 2 is finding the longest distance. Santa is showing off. Refactored the
// recursive permutation walker to use a comparator function pointer. Also, as
// seems to be common for me, macros and global variables.

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

#define MAX_CITIES      20

typedef char hash_t;

int city_count = 0;
char *cities[MAX_CITIES];
#define add_city(c)         city_count; cities[city_count++] = strdup(c)
#define city_from_hash(h)   (cities[h % city_count])

hash_t hash_from_city(char *city) {
    for (int i = 0; i < city_count; i++) {
        if (strcmp(city, cities[i]) == 0)
            return (hash_t)i;
    }
    return -1;
}

hash_t safe_add_city(char *city) {
    hash_t hash = 0;

    if ((hash = hash_from_city(city)) == -1) {
        hash = add_city(city);
    }

    return hash;
}

int distances[MAX_CITIES * MAX_CITIES];

#define get_distance(from, to)      *(distances + from + (to * MAX_CITIES))
#define set_distance(from, to, d)   *(distances + from + (to * MAX_CITIES)) = d

void add_distance(char *str) {
    int distance;
    char from[40], to[40];
    if (sscanf(str, "%s to %s = %d", from, to, &distance) < 3)
        fprintf(stderr, "error: cannot parse '%s'\n", str);
    hash_t f = safe_add_city(from);
    hash_t t = safe_add_city(to);

    set_distance(f, t, distance);
    set_distance(t, f, distance);
}

hash_t final_journey[MAX_CITIES];
int shortest_distance = 1000000;
int longest_distance = 0;

#define swap(a, b)      { hash_t tmp; tmp = a; a = b; b = tmp; }

int calculate_distance(hash_t *cities, int end) {
    int distance = 0;
    for (int i = 0; i < end; i++) {
        distance += get_distance(cities[i], cities[i + 1]);
    }

    return distance;
}

void explore_journey(hash_t *cities, int start, int end, bool (*compare)(int dist)) {
    if (start == end) {
        // see if the distance is shortest, if so then...
        int dist = calculate_distance(cities, end);
        if (compare(dist)) {
            memcpy(final_journey, cities, (end + 1) * sizeof(hash_t));
        }
    }
    else {
        for (int i = start; i <= end; i++) {
            swap(cities[i], cities[start]);
            explore_journey(cities, start + 1, end, compare);
            swap(cities[i], cities[start]);
        }
    }
}

bool shortest_comparator(int dist) {
    if (dist < shortest_distance) {
        shortest_distance = dist;
        return true;
    }
    return false;
}

bool longest_comparator(int dist) {
    if (dist > longest_distance) {
        longest_distance = dist;
        return true;
    }
    return false;
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    char arg[128];

    while (fgets(arg, sizeof(arg) - 1, input)) {
        trim(arg);
        add_distance(arg);
    }

    // dump the distance table
    printf("              ");
    for (int city = 0; city < city_count; city++)
        printf("- %-13s", city_from_hash(city));
    printf("\n");
    for (int r = 0; r < city_count; r++) {
        printf("%-13s - ", city_from_hash(r));
        for (int c = 0; c < city_count; c++) {
            char dist[20];
            sprintf(dist, "%d", get_distance(r,c));
            printf("%-15s", dist);
        }
        printf("\n");
    }

    // setup a working journey buffer for the permutations to be explored
    hash_t *working_journey = calloc(city_count, sizeof(hash_t));
    for (int i = 0; i < city_count; i++)
        working_journey[i] = i;

    // explore the permutations, with the comparator looking for the shortest
    explore_journey(working_journey, 0, city_count - 1, shortest_comparator);

    // Dump the final shortest journey
    printf("\nshortest distance %d: %s", shortest_distance, city_from_hash(final_journey[0]));
    for (int city = 1; city < city_count; city++)
        printf(" -%d- %s", get_distance(final_journey[city - 1], final_journey[city]), city_from_hash(final_journey[city]));

    // explore the permutations, with the comparator looking for the longest
    explore_journey(working_journey, 0, city_count - 1, longest_comparator);

    // Dump the final longest journey
    printf("\nlongest distance %d: %s", longest_distance, city_from_hash(final_journey[0]));
    for (int city = 1; city < city_count; city++)
        printf(" -%d- %s", get_distance(final_journey[city - 1], final_journey[city]), city_from_hash(final_journey[city]));

    return 0;
}
