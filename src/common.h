#ifndef COMMON_H
#define COMMON_H

struct each_file {
    char* name;
    char** lines;
    int line_count;
};

struct each_file get_struct(char* list_of_markdown_names);

char* parse_line(char** line, int n);

#endif
