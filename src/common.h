#ifndef COMMON_H
#define COMMON_H

struct each_file {
    char* name;
    char** lines;
    int line_count;
};

struct metadata {
    char key[32];
    char value[256];
};

struct frontmatter {
    struct metadata entries[32];

    int count;
};

struct each_file get_struct(char* list_of_markdown_names);

struct frontmatter *get_filled_metadata();

char* parse_line(char** line, int n);

#endif
