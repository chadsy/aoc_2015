// AOC 2015 solution in C
// @chadsy
// Copyright (C) 2021 Chad Royal
// MIT License http://opensource.org/licenses/MIT
//
// Day 14
// The one about reindeer running a tortoise and hare race for 2503 seconds.
//
// Is this really just about loops and states? Like, I 'run' each of the racers
// in a loop for 2503 iterations and toggle their run/rest states as appropriate?
// Nah, it's more like there are cycles of run time + rest time, with the distance
// being the run time * speed. Divide the total time by this cycle time and get
// the base distance. Then take any remainder and see if a run time window fits,
// and add that into the total distance (pro-rated as appropriate).
//
// First cut was Blitzen at 3230 km. No good, too high. Have to keep debugging.
// After fixing my math (what's new?) found the right answer of Vixen at 2660 km.
//
// Part 2 is to give points to the leading reindeer (multiple, if they're tied)
// for each second that they lead. That means going back to the brute-force
// iteration, incrementing the time, calculating the distance for each in the
// herd, and finding the leader after every second. Ugh.
//
// Blitzen at 1257 points is too high. Oops, off by 1 error in the seconds
// iteration loop. 1256 for Blitzen is the right answer.

#include <stdio.h>
#include <string.h>
#include <sys/param.h>

typedef struct {
    char *name;
    int runtime;
    int resttime;
    int speed;
    int distance;
    int points;
} reindeer;

#define RUN_TIME        2503
#define MAX_REINDEER    20

int herd_count = 0;
reindeer herd[MAX_REINDEER];

reindeer find_fastest_reindeer(void) {
    int fastest_index = 0;
    int longest = 0;

    for (int i = 0; i < herd_count; i++) {
        if (herd[i].distance > longest) {
            longest = herd[i].distance;
            fastest_index = i;
        }
    }

    return herd[fastest_index];
}

reindeer find_scoring_reindeer(void) {
    int fastest_index = 0;
    int score = 0;

    for (int i = 0; i < herd_count; i++) {
        if (herd[i].points > score) {
            score = herd[i].points;
            fastest_index = i;
        }
    }

    return herd[fastest_index];
}

int calculate_distance(reindeer r, int time) {
    int cycles = time / (r.runtime + r.resttime);
    int distance = cycles * r.speed * r.runtime;

    int remainder = time % (r.runtime + r.resttime);
    distance += MIN(r.runtime, remainder) * r.speed;

    return distance;
}

void calculate_points(void) {
    int distance = 0;

    for (int sec = 1; sec < RUN_TIME; sec++) {

        for (int r = 0; r < herd_count; r++) {
            herd[r].distance = calculate_distance(herd[r], sec);
            if (herd[r].distance > distance)
                distance = herd[r].distance;
        }

        for (int r = 0; r < herd_count; r++) {
            if (herd[r].distance == distance)
                herd[r].points++;
        }
    }
}

void load_reindeer(char *str) {
    char buf[20];
    reindeer r;
    memset(&r, 0, sizeof(r));

    int found = sscanf(str, "%s can fly %d km/s for %d seconds, but then must rest for %d seconds.\n",
        buf, &r.speed, &r.runtime, &r.resttime);

    if (found != 4)
        fprintf(stderr, "error: cannot parse '%s'", str);
    else {
        r.name = strdup(buf);

        r.distance = calculate_distance(r, RUN_TIME);
        herd[herd_count++] = r;
    }
}

void dump_herd(void) {
    for (int i = 0; i < herd_count; i++) {
        reindeer r = herd[i];
        printf("%s %d km/s for %d, rests for %d, can travel %d km for %d points\n",
            r.name, r.speed, r.runtime, r.resttime, r.distance, r.points);
    }
}

int main(int argc, char **argv) {
    FILE *input = stdin;
    char arg[128];

    while (fgets(arg, sizeof(arg) - 1, input)) {
        load_reindeer(arg);
    }

    reindeer fastest = find_fastest_reindeer();
    // dump_herd();

    printf("Part 1: %s traveled %d km\n", fastest.name, fastest.distance);

    calculate_points();
    fastest = find_scoring_reindeer();
    // dump_herd();
    printf("Part 2: %s scored %d points\n", fastest.name, fastest.points);

    return 0;
}
