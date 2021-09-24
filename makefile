
BUILD_FOLDER=build
INPUTS_FOLDER=inputs

# all :
#   @echo Please specify a specific target to run, e.g. 'day03' which will build and run the puzzle.

buildall : $(BUILD_FOLDER)/day01.app $(BUILD_FOLDER)/day02.app $(BUILD_FOLDER)/day03.app \
		   $(BUILD_FOLDER)/day04.app $(BUILD_FOLDER)/day05.app

.PHONY : clean buildall runall \
		 day01 day02 day03 day04 day05

runall : day01 day02 day03 day04 day05

CC=clang
CFLAGS=-g

clean :
	@echo Clean: removing *.app files from /$(BUILD_FOLDER) folder
	@rm -f $(BUILD_FOLDER)/*.app

# TODO: Use inference rules, these are almost all identical so far
# TODO: Use a project list macro too

day01 : $(BUILD_FOLDER)/day01.app $(INPUTS_FOLDER)/day01.txt
	$(BUILD_FOLDER)/day01.app < $(INPUTS_FOLDER)/day01.txt

$(BUILD_FOLDER)/day01.app : day01.c
	$(CC) $(CFLAGS) day01.c -o $(BUILD_FOLDER)/day01.app

day02 : $(BUILD_FOLDER)/day02.app $(INPUTS_FOLDER)/day02.txt
	$(BUILD_FOLDER)/day02.app < $(INPUTS_FOLDER)/day02.txt

$(BUILD_FOLDER)/day02.app : day02.c
	$(CC) $(CFLAGS) day02.c -o $(BUILD_FOLDER)/day02.app

day03 : $(BUILD_FOLDER)/day03.app $(INPUTS_FOLDER)/day03.txt
	$(BUILD_FOLDER)/day03.app < $(INPUTS_FOLDER)/day03.txt

$(BUILD_FOLDER)/day03.app : day03.c
	$(CC) $(CFLAGS) day03.c -o $(BUILD_FOLDER)/day03.app

day04 : $(BUILD_FOLDER)/day04.app $(INPUTS_FOLDER)/day04.txt
	$(BUILD_FOLDER)/day04.app < $(INPUTS_FOLDER)/day04.txt

$(BUILD_FOLDER)/day04.app : day04.c
	$(CC) $(CFLAGS) day04.c -o $(BUILD_FOLDER)/day04.app -lssl -lcrypto

day05 : $(BUILD_FOLDER)/day05.app $(INPUTS_FOLDER)/day05.txt
	$(BUILD_FOLDER)/day05.app < $(INPUTS_FOLDER)/day05.txt

$(BUILD_FOLDER)/day05.app : day05.c
	$(CC) $(CFLAGS) day05.c -o $(BUILD_FOLDER)/day05.app

