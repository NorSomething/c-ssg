#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

int heading_counter(char* line, int* amount_to_skip) {

    if (line == NULL)
        return -1;

    int count = 0;

    while(line[count] == '#') {
        count++;
    }

    *amount_to_skip = count;
    if (count > 0) {
        while(line[*amount_to_skip] == ' ') {
            (*amount_to_skip)++;
        }
    }

    return count;
}

char* bold_italics_line_giver(char *line) {

    if (line == NULL)
        return NULL;

    size_t line_len = strlen(line);

    char* output = malloc((line_len*3+1)*sizeof(char)); //3x buffer size to account for html tags
    if (output == NULL) {
        perror("Error when bold italics..\n");
        exit(1);
    }

    size_t line_reader_pointer = 0;
    size_t output_writer_pointer = 0;

    while (line_reader_pointer < line_len) {

        //bold check
        if (line_reader_pointer + 1 < line_len && line[line_reader_pointer] == '*' && line[line_reader_pointer+1] == '*') {

            //finding closing **
            char* closing = strstr(&line[line_reader_pointer+2], "**");
            if (closing != NULL) {

                //opening tag
                strcpy(&output[output_writer_pointer], "<b>");
                output_writer_pointer += 3;
                line_reader_pointer += 2;

                while (&line[line_reader_pointer] != closing) {
                    output[output_writer_pointer++] = line[line_reader_pointer++];
                }

                //closing tag
                strcpy(&output[output_writer_pointer], "</b>");
                output_writer_pointer += 4;
                line_reader_pointer += 2;

            }

            else {

                //normal text
                output[output_writer_pointer++] = line[line_reader_pointer++];

            }

        }

        //italics check
        else if (line[line_reader_pointer] == '*') {

            char* closing = strchr(&line[line_reader_pointer+1], '*');

            if (closing != NULL && *(closing+1) != '*') { //not bold

                strcpy(&output[output_writer_pointer], "<i>");
                output_writer_pointer += 3;
                line_reader_pointer += 1;

                while (&line[line_reader_pointer] != closing) {
                   output[output_writer_pointer++] = line[line_reader_pointer++];
                }

                //closing
                strcpy(&output[output_writer_pointer], "</i>");
                output_writer_pointer += 4;
                line_reader_pointer += 1;

            }
            else {
                output[output_writer_pointer++] = line[line_reader_pointer++];
            }

        }

        else {

            //normal chars
            output[output_writer_pointer++] = line[line_reader_pointer++];

        }

    }

    output[output_writer_pointer] = '\0';
    return output;

}

char* parse_line(char** line, int n) {

    size_t buffer_size = 0;

    for (int i = 0; i < n; i++) {
        buffer_size += (strlen(line[i])*3+16); //x3 for worst case html tag expansion, 16 for closing
    }

    // output string in heap so that it survies in main.c
    char* output = malloc((buffer_size+1)*sizeof(char));
    if (output == NULL) {
        perror("Error when making output string...\n");
        exit(1);
    }

    output[0] = '\0';

    for (int i = 0; i < n; i++) {

        char temp[1024];
        int amount_to_skip_heading = 0;
        int hash_count = heading_counter(line[i], &amount_to_skip_heading);

        char* bolded_italics_text = bold_italics_line_giver(line[i] + amount_to_skip_heading);

        int len_to_check = strlen(bolded_italics_text);

        if (hash_count > 0 && hash_count <= 6 && len_to_check > 0) {
            snprintf(temp, sizeof(temp), "<h%d>%s</h%d>\n", hash_count, bolded_italics_text, hash_count);
        }
        else {
            //if incase we have to include the ** and *
            free(bolded_italics_text);
            bolded_italics_text = bold_italics_line_giver(line[i]);
            snprintf(temp, sizeof(temp), "<p>%s</p>\n", bolded_italics_text);
        }

        strcat(output, temp);
        free(bolded_italics_text);

    }


    if (output != NULL) {
        return output;
    }
    else {
        return NULL;
    }

}
