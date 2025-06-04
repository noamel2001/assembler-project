CC = gcc
CFLAGS = -Wall -pedantic -std=c99
TARGET = pre_assembler
SRC = pre_assembler.c
INPUT = input.as
OUTPUT = expanded_code.am

all: $(TARGET)

$(TARGET): $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o $(TARGET)

run: $(TARGET)
	./$(TARGET) < $(INPUT)

# === בדיקות ===

test-valid1:
	./$(TARGET) tests/valid1.as

test-valid2:
	./$(TARGET) tests/valid2.as

test-valid3:
	./$(TARGET) tests/valid3.as

test-error1:
	./$(TARGET) tests/error1.as

test-error2:
	./$(TARGET) tests/error2.as

test-error3:
	./$(TARGET) tests/error3.as

# מריץ את כל התקינים
test-valid: test-valid1 test-valid2 test-valid3

# מריץ את כל השגויים
test-error: test-error1 test-error2 test-error3

# הכל
test-all: test-valid test-error

clean:
	rm -f $(TARGET).exe $(OUTPUT)
assembler: assembler.c
	gcc -Wall -ansi -pedantic assembler.c -o assembler

