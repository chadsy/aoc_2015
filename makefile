BUILD_FOLDER=build
INPUTS_FOLDER=inputs

CC=clang
CFLAGS=-g

all : day01 day02

clean :
	@echo Clean: removing *.app files from /$(BUILD_FOLDER) folder
	@rm -f $(BUILD_FOLDER)/*.app

day01 : $(BUILD_FOLDER)/day01.app $(INPUTS_FOLDER)/day01.txt
	$(BUILD_FOLDER)/day01.app < $(INPUTS_FOLDER)/day01.txt

$(BUILD_FOLDER)/day01.app : day01.c
	$(CC) $(CFLAGS) day01.c -o $(BUILD_FOLDER)/day01.app

day02 : $(BUILD_FOLDER)/day02.app $(INPUTS_FOLDER)/day02.txt
	$(BUILD_FOLDER)/day02.app < $(INPUTS_FOLDER)/day02.txt

$(BUILD_FOLDER)/day02.app : day02.c
	$(CC) $(CFLAGS) day02.c -o $(BUILD_FOLDER)/day02.app
