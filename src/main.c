#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "common.h"

int main() {

    char list_of_markdown_names[127][128];
    int markdown_names_index = 0;

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

        strncpy(list_of_markdown_names[markdown_names_index], de->d_name, 128-1);
        list_of_markdown_names[markdown_names_index][128-1] = '\0';

        markdown_names_index++;

    }

    closedir(dir_stream);

    printf("Found %d markdown files. \n", markdown_names_index);

    for (int i = 0; i < markdown_names_index; i++) {

        struct each_file file_data = get_struct(list_of_markdown_names[i]);

        if (file_data.lines == NULL) {
            printf("Skipping %s : read error.\n", file_data.name);
            continue;
        }

        printf("Contents of %s (%d lines) : \n", file_data.name, file_data.line_count);

        for (int j = 0; j < file_data.line_count; j++) {
            printf("%s\n", file_data.lines[j]);
        }

        char *parsed_line = parse_line(file_data.lines, file_data.line_count);

        printf("Parsed line of %s : \n", file_data.name);

       printf("%s\n", parsed_line);

        //cleanup

        if (parsed_line != NULL) {
            free(parsed_line);
        }

        if (file_data.lines != NULL) {
            for (int j = 0; j < file_data.line_count; j++) {
                free(file_data.lines[j]);
            }
            free(file_data.lines); //freeing individual lines, then the whole array itself
        }

    }



    return 0;
}
