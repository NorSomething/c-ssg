#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"


struct each_file file;

char* parse_line(char** line, int n) {

    size_t buffer_size = 0;

    for (int i = 0; i < n; i++) {
        buffer_size += (strlen(line[i])+8);
    }

    // output string in heap so that it survies in main.c
    char* output = malloc((buffer_size+1)*sizeof(char));
    if (output == NULL) {
        perror("Error when making output string...\n");
        exit(1);
    }

    output[0] = '\0';

    for (int i = 0; i < n; i++) {

        char temp[256];
        snprintf(temp, sizeof(temp), "<p>%s</p>\n", line[i]);
        strcat(output, temp);

    }


    if (output != NULL) {
        return output;
    }
    else {
        return NULL;
    }

}
