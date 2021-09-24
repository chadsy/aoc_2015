# Advent of Code 2015: Code Kata in C

I'm solving the [Advent of Code](https://adventofcode.com/2015) puzzles from 2015 using simple C, as a kind of daily code kata. I intend to stick with bog-standard ANSI-C89 for the most part, except for the single-line comment style. So maybe we call it C89-ish, but then there are also some later libc and 3rd party things that I've used. I have no idea what kind of patterns and idioms will creep in, as my head's version of C programming is a mix of like 20 different languages since I first learned and used pre-ANSI C.

A brief aside: I am so grateful for these annual puzzles. [Eric Wastl](http://was.tl/) deserves tremendous recognition for his annual project. Going back to this first year, I was struck how completely identical the overall process and structure of the collection was implemented. Like, he knew at the outset what was going to work well and was virtually completely right about it. Well done, Eric.

I'm using a simple editor and terminal, with a simple makefile, and an idiomatic CLI `stdin` model for inputs so I can test using pipes and redirections. And I'm trying to not rely on code navigation features or statement completion or a debugger or a profiler.

Here at the beginning, I am thinking I might add more complexity as I go along. Maybe by adding additional makefile targets for testing, based on simple parsing of outputs, etc. Then I could go back and add timings and multiple iterations and experiment with optimizing, both big O analysis and applied profiling.

# Project Layout

The project root folder contains a series of `dayXX.c` files, each source file contains solutions for both the part 1 and part 2 challenges in the puzzle for that day. The `./inputs` folder contains the input data provided for each puzzle, each one used for both parts 1 and 2. The `./build` folder contains the compiled programs as `.app` files. So running one looks something like:

```bash
> echo "2x3x4" | build/day02.app
total paper: 58
total ribbon: 34

> build/day02.app < inputs/day02.txt
total paper: 12345
total ribbon: 67890
```

# Makefile

The makefile is a work-in-progress. It will get unwieldy, so there are TODOs to simplify things using inference rules and target lists. For now, there are few basic targets:

* `dayXX` will build and run that specific day's app. This is the sweet spot for testing, just `make day02`, etc.
* `build/dayXX.app` will build that specific day's app as needed
* `clean` just what it says, which is to clear out the /build folder where the resulting apps are stored
* `buildall` will build all the apps as needed, but not run them
* `runall` will run all the apps, which will also build them as needed

# Dependencies

* `gcc` is used in the makefile, which for xcode users like me maps to `clang v12.0.5`. I didn't test with an actual gcc or other versions, etc.
* `make` is used, but should be super-basic. If it's not fully portable, it's bog-simple to understand and replicate.
* `openSSL` is used in Day 04 for its MD5 implementation, as mentioned in the extra notes below.

# License

All this is under [MIT License](http://opensource.org/licenses/MIT), I read somewhere that if Lizzo had a software license it would be MIT.

# Some Extra Implementation Notes

## Day 04
This is the one about MD5. Deciding on and expanding to a library to implement MD5 was a thing. Looking at MD5 implementations left me with the notion that while it's a solved problem it's also still 'some work' to calculate, even on the small strings in the puzzle. So given O(n) and merely counting up from 0 and testing each result for the matching pattern, I expected performance to be an issue. General optimization was on my mind. Once the second part of the puzzle was revealed, I decided to attempt several optimizations and time the results. At the end, both parts are solved by spinning through the integers once and evaluating both parts inside the same loop. I decided to time that (approx 1.6 sec for both parts together). I also decided to implement the solution using byte-wise evaluation instead of string evaluation. That meant not having to render the MD5 hash until I needed to show the result string. Calculating those two took approx 0.27 sec. It's not interesting to know the hardware here, but merely to say that the binary evalation was just over 5x faster. I thought it would have been even faster.

>NB: I chose the openSSL MD5 implementation. I hadn't coded against it before, so there was some setup for the headers+library needed. That includes symlinks for the libraries and header folders, and extra params on the build in the makefile.

## Day 06
On the first part of the puzzle, the lights were simply on or off; a bool is fine for the array of lights. For the second part, there weren't any requirements that specified the range of values possible. The only inferrable constraint is that the values were always positive, so an unsigned type would work. If we see that there are 300 operations specified, we can rationally expect that the brightness value of a single light would never exceed 255. So an unsigned char would work for storage.

However, I got obsessed with avoiding the constraint. I had factored the code to handle different storage types but if I used anything other than `char` or `unsigned char` in the `typedef char light;` definition, I got wrong and different answers:

```text
    using lights of type 'char'
    Part 1, lights remaining on 569999
    Part 2, total brightness 17836115

    using lights of type 'short'
    Part 1, lights remaining on 570888
    Part 2, total brightness 17891241

    using lights of type 'int'
    Part 1, lights remaining on 572474
    Part 2, total brightness 18001471

    using lights of type 'long'
    Part 1, lights remaining on 575036
    Part 2, total brightness 18221659
```
So obviously I have a bug in my pointer arithmetic, or in double-compensating for memory spans vs letting the compiler handle pointer arithmetic. Once fixed, even `double` worked too:
```text
    using lights of type 'double'
    Part 1, lights remaining on 569999
    Part 2, total brightness 17836115
```
