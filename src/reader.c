#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

struct each_file {
    char* name;
    char** lines;
    int line_count;
};
typedef struct each_file f;

int get_markdown_names(char list_of_markdown_names[][128], int* markdown_names_index) {

    struct dirent *de; //dirent entry
    DIR *dir_stream = opendir("../markdowns");
    if (dir_stream == NULL) {
        perror("Error while opening the markdown directory");
        exit(1);
    }

    while(((de = readdir(dir_stream)) != NULL)) {

        if ((strcmp(de->d_name, ".") == 0) || (strcmp(de->d_name, "..") == 0)) {
            continue;
        }

        size_t len = strlen(de->d_name);

        if (len < 3 || (strcmp(de->d_name + len - 2, "md") != 0)) {
            continue;
        }

        strncpy(list_of_markdown_names[*markdown_names_index], de->d_name, 128-1);
        list_of_markdown_names[*markdown_names_index][128-1] = '\0';

        (*markdown_names_index)++;

    }

    closedir(dir_stream);

    return 0;

}

char** file_reader(char* name, int* out_line_count) {

    FILE* fp = fopen(name, "r");

    if (fp == NULL) {
        printf("Error in opening markdown file.\n");
        return NULL;
    }

    int capacity = 10;
    int count = 0;
    char **lines = malloc(capacity*sizeof(char*));

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
        strcpy(lines[count], current_line);
        count++;


    }

    free(current_line);
    fclose(fp);

    *out_line_count = count;
    return lines;

}

int main() {

    char list_of_markdown_names[127][128];
    int markdown_names_index = 0;

    int x = get_markdown_names(list_of_markdown_names, &markdown_names_index);

    char* file_1 = list_of_markdown_names[0];
    f file1;

    int line_count = 0; //temp

    char path_buffer[128];
    snprintf(path_buffer, sizeof(path_buffer), "../markdowns/%s", list_of_markdown_names[0]);

    char **lines = file_reader(path_buffer, &line_count);

    file1.lines = lines;
    file1.name = file_1;
    file1.line_count = line_count;

    //error handling add later

    for (int i = 0; i < line_count; i++) {
        printf("%s\n", lines[i]);
    }

    for (int i = 0; i < line_count; i++) {
        free(lines[i]);
    }

    free(lines);

    return 0;
}
