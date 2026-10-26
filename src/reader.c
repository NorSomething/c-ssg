#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "common.h"

char** file_reader(char* name, int* out_line_count) {

    FILE* fp = fopen(name, "r");

    if (fp == NULL) {
        printf("Error in opening markdown file.\n");
        return NULL;
    }

    int capacity = 10;
    int count = 0;
    char **lines = malloc(capacity*sizeof(char*));
    if (lines == NULL) {
        fclose(fp);
        return NULL;
    }

    char *current_line = NULL;
    size_t line_buffer_size = 0;
    ssize_t line_length;

    while ((line_length = getline(&current_line, &line_buffer_size, fp)) != -1) {

        //if full
        if (count >= capacity) {
            capacity *= 2; //standard Cpp vector style behaviour
            char **temp = realloc(lines, capacity*sizeof(char*));
            if (temp == NULL) {
                printf("Realloc failed...\n ");
                fclose(fp);
                free(current_line);
                for (int i = 0; i < count; i++) free(lines[i]);
                free(lines);
                return NULL;
            }
            lines = temp;
        }

        //removing \n
        if (line_length > 0 && current_line[line_length-1] == '\n') {
            current_line[line_length-1] = '\0';
        }

        //lines live here
        lines[count] = malloc((strlen(current_line)+1)*sizeof(char));
        if (lines[count] == NULL) {
            fclose(fp);
            free(current_line);
            for (int i = 0; i < count; i++) free(lines[i]);
            free(lines);
            return NULL;
        }
        strcpy(lines[count], current_line);
        count++;


    }

    free(current_line);
    fclose(fp);

    *out_line_count = count;
    return lines;

}

struct each_file get_struct(char* markdown_file_name) {

    struct each_file markdown_file;
    int line_count = 0;

    char path_buffer[128];
    int path_length = snprintf(path_buffer, sizeof(path_buffer), "markdowns/%s", markdown_file_name);
    if (path_length < 0 || (size_t)path_length >= sizeof(path_buffer)) {
        markdown_file.lines = NULL;
        markdown_file.name = markdown_file_name;
        markdown_file.line_count = 0;
        return markdown_file;
    }

    char **lines = file_reader(path_buffer, &line_count);

    markdown_file.lines = lines;
    markdown_file.name = markdown_file_name;
    markdown_file.line_count = line_count;

    return markdown_file;

    //error handling add later

    /*
    for (int i = 0; i < line_count; i++) {
        printf("%s\n", lines[i]);
    }

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }

    free(lines);
    */

    //return 0;
}
