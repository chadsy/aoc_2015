# Advent of Code 2015: Code Kata in C

I'm solving the [Advent of Code](https://adventofcode.com/2015) puzzles from 2015 using simple C, as a kind of daily code kata. I intend to stick with bog-standard C89 for the most part, except for the single-line comment style. So maybe we call it C89-ish, but then there are also some later libc things that I've used. I have no idea what kind of patterns and idioms will creep in, as my head's version of C programming is a mix of like 20 different languages since I first learned and used pre-ANSI C.

A brief aside: I am so grateful for these annual puzzles. [Eric Wastl](http://was.tl/) deserves tremendous recognition for his annual project. Going back to this first year, I was struck how completely identical the overall process and structure of the collection was implemented. Like, he knew at the outset what was going to work well and was virtually completely right about it. Well done, Eric.

I'm using a simple editor and terminal, with a simple makefile, and an idiomatic CLI `stdin` model for inputs so I can test using pipes and redirections. And I'm trying to not rely on code navigation features or statement completion.

The project root folder contains a series of `dayXX.c` files, each source file contains solutions for both the part 1 and part 2 challenges in the puzzle for that day. The `./inputs` folder contains the input data provided for each puzzle, each one used for both parts 1 and 2. The `./build` folder contains the compiled programs as `.app` files. So running one looks something like:

```bash
> echo "2x3x4" | build/day02.app
total paper: 58
total ribbon: 34

> build/day02.app < inputs/day02.txt
total paper: 12345
total ribbon: 67890
```

Here at the beginning, I am thinking I might add more complexity as I go along. Maybe by adding additional makefile targets for testing, based on simple parsing of outputs, etc. Then I could go back and add timings and multiple iterations and experiment with optimizing, both big O analysis and applied profiling.

All this is under [MIT License](http://opensource.org/licenses/MIT), I read somewhere that if Lizzo had a software license it would be MIT.
