#include <stdio.h>
#include <string.h>
#include <ctype.h>

#define MAX_LINE_LEN 81
#define MAX_LINES 500

typedef struct {
    char label[32];
    char command[16];
    char operand1[32];
    char operand2[32];
} ParsedLine;

char *trim(char *str) {
    char *end;
    while (isspace(*str)) str++;
    if (*str == 0) return str;
    end = str + strlen(str) - 1;
    while (end > str && isspace(*end)) end--;
    *(end + 1) = '\0';
    return str;
}

int parse_line(const char *line, ParsedLine *pl) {
    char temp_line[MAX_LINE_LEN];
    int matched;

    strcpy(temp_line, line);
    char *trimmed = trim(temp_line);

    // התעלמות משורות ריקות או הערות
    if (trimmed[0] == '\0' || trimmed[0] == '\n' || trimmed[0] == ';') {
        return 0;
    }

    pl->label[0] = '\0';
    pl->command[0] = '\0';
    pl->operand1[0] = '\0';
    pl->operand2[0] = '\0';

    matched = sscanf(trimmed, "%31[^:]: %15s %31[^,], %31s",
                     pl->label, pl->command, pl->operand1, pl->operand2);
    if (matched == 4) return 1;

    matched = sscanf(trimmed, "%15s %31[^,], %31s",
                     pl->command, pl->operand1, pl->operand2);
    if (matched == 3) return 1;

    matched = sscanf(trimmed, "%15s %31s",
                     pl->command, pl->operand1);
    if (matched == 2) return 1;

    matched = sscanf(trimmed, "%15s",
                     pl->command);
    return (matched == 1);
}

int main() {
    FILE *src = fopen("expanded_code.am", "r");
    char line[MAX_LINE_LEN];
    ParsedLine parsed_lines[MAX_LINES];
    int line_number = 0;
    int parsed_count = 0;

    if (!src) {
        perror("Failed to open expanded_code.am");
        return 1;
    }

    while (fgets(line, MAX_LINE_LEN, src)) {
        line_number++;

        ParsedLine pl;

        if (parse_line(line, &pl)) {
            parsed_lines[parsed_count++] = pl;

            // הדפסה לבדיקה
            printf("Line %d: ", line_number);
            if (strlen(pl.label)) {
                printf("Label='%s' ", pl.label);
            }
            printf("Command='%s'", pl.command);
            if (strlen(pl.operand1)) {
                printf(", Operand1='%s'", pl.operand1);
            }
            if (strlen(pl.operand2)) {
                printf(", Operand2='%s'", pl.operand2);
            }
            printf("\n");
        } else {
            fprintf(stderr, "Error parsing line %d\n", line_number);
        }
    }

    fclose(src);
    return 0;
}
