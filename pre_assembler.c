#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 81
#define MAX_MACRO_NAME 31
#define MAX_MACROS 100
#define MAX_MACRO_LINES 200

typedef struct {
    char name[MAX_MACRO_NAME];
    char *lines[MAX_MACRO_LINES];
    int line_count;
} Macro;

Macro macros[MAX_MACROS];
int macro_count = 0;

/* Trim leading/trailing whitespace */
char *trim(char *str) {
    char *end;
    while (isspace(*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
    return str;
}

/* Check if macro exists */
int find_macro(const char *name) {
    for (int i = 0; i < macro_count; i++) {
        if (strcmp(macros[i].name, name) == 0)
            return i;
    }
    return -1;
}

/* Add new macro */
int add_macro(const char *name) {
    if (macro_count >= MAX_MACROS)
        return -1;
    strncpy(macros[macro_count].name, name, MAX_MACRO_NAME - 1);
    macros[macro_count].name[MAX_MACRO_NAME - 1] = '\0';
    macros[macro_count].line_count = 0;
    return macro_count++;
}

/* Add line to macro */
void add_line_to_macro(int idx, const char *line) {
    if (macros[idx].line_count < MAX_MACRO_LINES) {
        size_t len = strlen(line);
        char *line_copy = malloc(len + 2); // מקום ל־\n ול־\0
        strcpy(line_copy, line);
        if (line_copy[len - 1] != '\n') {
            line_copy[len] = '\n';
            line_copy[len + 1] = '\0';
        }
        macros[idx].lines[macros[idx].line_count] = line_copy;
        macros[idx].line_count++;
    }
}


int is_blank_or_comment(const char *line) {
    while (*line) {
        if (*line == ';') return 1;
        if (!isspace(*line)) return 0;
        line++;
    }
    return 1;
}

int main(int argc, char *argv[]) {
    FILE *src;
    FILE *out = fopen("expanded_code.am", "w");
    char line[MAX_LINE_LEN];
    int in_macro = 0, curr_macro = -1;
    int line_number = 0;

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file>\n", argv[0]);
        return 1;
    }

    src = fopen(argv[1], "r");

    if (!src || !out) {
        perror("File error");
        return 1;
    }

    while (fgets(line, MAX_LINE_LEN, src)) {
        char *trimmed = trim(line);
        line_number++;

        if (is_blank_or_comment(trimmed)) {
            fputs(line, out);
            continue;
        }

        if (strncmp(trimmed, "mcro", 4) == 0 && !in_macro) {
            char *name = trim(trimmed + 4);

            if (strlen(name) == 0) {
                fprintf(stderr, "Error on line %d: macro name missing after 'mcro'.\n", line_number);
                continue;
            }
            if (in_macro) {
                fprintf(stderr, "Error on line %d: nested macro definition is not allowed.\n", line_number);
                continue;
            }
            if (find_macro(name) != -1) {
                fprintf(stderr, "Error on line %d: Macro '%s' already defined.\n", line_number, name);
                continue;
            }

            curr_macro = add_macro(name);
            in_macro = 1;
        }
        else if (strncmp(trimmed, "mcroend", 7) == 0) {
            if (!in_macro) {
                fprintf(stderr, "Error on line %d: 'mcroend' found without matching 'mcro'.\n", line_number);
                continue;
            }
            in_macro = 0;
            curr_macro = -1;
        }
        else if (in_macro && curr_macro != -1) {
            add_line_to_macro(curr_macro, line);
        }
        else {
            char temp_line[MAX_LINE_LEN];
            strcpy(temp_line, trimmed);
            char *first_word = strtok(temp_line, " \t");

            int idx = find_macro(first_word);
            if (idx != -1 && strcmp(first_word, trimmed) == 0) {
                for (int i = 0; i < macros[idx].line_count; i++) {
                    fputs(macros[idx].lines[i], out);
                    size_t len = strlen(macros[idx].lines[i]);
                    if (len == 0 || macros[idx].lines[i][len - 1] != '\n') {
                        fputc('\n', out); // מוסיף ירידת שורה אם חסרה
                    }
                }

            } else {
                fputs(line, out);
            }
        }
    }

    if (in_macro) {
        fprintf(stderr, "Error: macro definition was not closed with 'mcroend'.\n");
    }

    fclose(src);
    fclose(out);
    return 0;
}
