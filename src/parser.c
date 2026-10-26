#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "common.h"

struct frontmatter fm;

struct frontmatter* get_filled_metadata() {
    return &fm;
}

//this funciton is AI's idea for safe C :fire:
static void append_escaped_text(char *out, size_t capacity, size_t *used, const char *text) {
    for (const unsigned char *p = (const unsigned char *)text; *p != '\0'; p++) {
        const char *entity = NULL;
        if (*p == '&') entity = "&amp;";
        else if (*p == '<') entity = "&lt;";
        else if (*p == '>') entity = "&gt;";
        if (entity != NULL) {
            size_t n = strlen(entity);
            if (n >= capacity - *used) return;
            memcpy(out + *used, entity, n);
            *used += n;
        } else {
            if (*used + 1 >= capacity) return;
            out[(*used)++] = (char)*p;
        }
    }
    out[*used] = '\0';
}

static int store_metadata(const char *key, const char *value) {
    if (fm.count >= (int)(sizeof(fm.entries) / sizeof(fm.entries[0])) || strlen(key) >= sizeof(fm.entries[0].key) ||strlen(value) >= sizeof(fm.entries[0].value)) {
        fprintf(stderr, "Warning: frontmatter entry is too large or there are too many entries; skipping it.\n");
        return 0;
    }
    snprintf(fm.entries[fm.count].key, sizeof(fm.entries[fm.count].key), "%s", key);
    snprintf(fm.entries[fm.count].value, sizeof(fm.entries[fm.count].value), "%s", value);
    fm.count++;
    return 1;
}

char* parse_links(char* line) {

    if (line == NULL)
        return NULL;

    size_t line_len = strlen(line);
    if (line_len > (SIZE_MAX - 512) / 128) return NULL;
    char* output = malloc(line_len * 128 + 512);
    if (output == NULL) {
        perror("Error when parsing links\nExiting...\n");
        exit(1);
    }

    //same tech again
    size_t line_reader_pointer = 0;
    size_t output_writer_pointer = 0;

    while(line_reader_pointer < line_len) {

        if (line_reader_pointer + 1 < line_len && line[line_reader_pointer] == '[') {
            char* closing_link_name = strchr(&line[line_reader_pointer+1], ']');

            if (closing_link_name != NULL && *(closing_link_name+1) == '(') {
                char* closing_link_addr = strchr(closing_link_name+2, ')');

                if (closing_link_addr != NULL) {

                    size_t link_name_len = closing_link_name - (&line[line_reader_pointer+1]);
                    size_t link_addr_len = closing_link_addr - (closing_link_name+2);

                    char link_name[256];
                    char link_addr[256];

                    if (link_name_len < sizeof(link_name) && link_addr_len < sizeof(link_addr)) {

                        strncpy(link_name, &line[line_reader_pointer+1], link_name_len);
                        link_name[link_name_len] = '\0';

                        strncpy(link_addr, closing_link_name+2, link_addr_len);
                        link_addr[link_addr_len] = '\0';

                        char final_html[544];
                        snprintf(final_html, sizeof(final_html), "<a class=\"links\" href=\"%s\">%s</a>", link_addr, link_name);
                        size_t final_html_len = strlen(final_html);

                        strcpy(&output[output_writer_pointer], final_html);
                        output_writer_pointer += final_html_len;

                        //moving pointer past []() stuff
                        line_reader_pointer = (closing_link_addr - line) + 1;
                        continue;

                    }
                }
            }
        }
        output[output_writer_pointer++] = line[line_reader_pointer++];
    }

    output[output_writer_pointer] = '\0';
    return output;

}

char* parse_videos(char* line) {

    if (line == NULL)
        return NULL;

    size_t line_len = strlen(line);

    if (line_len > (SIZE_MAX - 512) / 128) return NULL;
    size_t capacity = line_len * 128 + 512;
    char* output = malloc(capacity);

    if (output == NULL) {
        perror("Error when parsing videos\nExiting...\n");
        exit(1);
    }

    //same tech, once again
    size_t line_reader_pointer = 0;
    size_t output_writer_pointer = 0;

    while (line_reader_pointer < line_len) {

        if (line_reader_pointer + 5 < line_len && strncmp(&line[line_reader_pointer], "~vid=", 5) == 0) {
            char* closing = strchr(&line[line_reader_pointer+5], '~');

            if (closing != NULL) {
                size_t vid_name_len = closing - (&line[line_reader_pointer+5]) ;

                char vid_name[256];
                if (vid_name_len < sizeof(vid_name)) {
                    strncpy(vid_name, &line[line_reader_pointer+5], vid_name_len);
                    vid_name[vid_name_len] = '\0';

                    char vid_html[720];
                    snprintf(vid_html, sizeof(vid_html), "<video controls width=460 class=\"site-vids\">\n <source src=../site-imgs/%s type=\"video/mp4\">\n <source src=../site-imgs/%s type=\"video/webm\"> \nYour browser does not support vid tag (fallback)\n</video>", vid_name, vid_name);
                    size_t vid_html_len = strlen(vid_html);
                    strcpy(&output[output_writer_pointer], vid_html);
                    output_writer_pointer += vid_html_len;

                    //moving past filename
                    line_reader_pointer += (5 + vid_name_len + 1);
                    continue;
                }
            }
        }

        output[output_writer_pointer++] = line[line_reader_pointer++];
    }

    output[output_writer_pointer] = '\0';
    return output;

}

char* parse_images(char* line) {

    if (line == NULL)
        return NULL;

    size_t line_len = strlen(line);
    if (line_len > (SIZE_MAX - 512) / 128) return NULL;
    char* output = malloc(line_len * 128 + 512);
    if (output == NULL) {
        perror("Error when parsing images\nExiting...\n");
        exit(1);
    }

    //same tech as bold italics func
    size_t line_reader_pointer = 0;
    size_t output_writer_pointer = 0;

    while (line_reader_pointer < line_len) {

        if (line_reader_pointer + 5 < line_len && strncmp(&line[line_reader_pointer], "~img=", 5) == 0) {
            char* closing = strchr(&line[line_reader_pointer+5], '~');

            if (closing != NULL) {
                size_t img_name_len = closing - (&line[line_reader_pointer+5]);

                char img_name[256];
                if (img_name_len < sizeof(img_name)) {
                    strncpy(img_name, &line[line_reader_pointer+5], img_name_len);
                    img_name[img_name_len] = '\0';

                    char img_html[576];
                    snprintf(img_html, sizeof(img_html), "<img src=\"../site-imgs/%s\" alt=\"%s\" class=\"site-imgs\">", img_name, img_name);
                    size_t img_html_len = strlen(img_html);
                    strcpy(&output[output_writer_pointer], img_html);
                    output_writer_pointer += img_html_len;

                    //moving poointer past filename
                    line_reader_pointer += (5 + img_name_len + 1);
                    continue;
                }
            }
        }

        output[output_writer_pointer++] = line[line_reader_pointer++];

    }

    output[output_writer_pointer] = '\0';
    return output;

}

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

int list_item_checker(char* line, int* amount_to_skip) {

    if (line == NULL)
        return 0;

    int i = 0;
    while(line[i] == ' ')
        i++;

    if ((line[i] == '-' || line[i] == '*' || line[i] == '+') && line[i+1] == ' ') {
        *(amount_to_skip) = i+2;
        return 1;
    }

    return 0;

}

char* single_line_codeblock_giver(char* line) {

    if (line == NULL)
        return NULL;

    size_t line_len = strlen(line);

    if (line_len > (SIZE_MAX - 1) / 128) return NULL;
    size_t output_capacity = line_len * 128 + 1;
    char* output = malloc(output_capacity);
    if (output == NULL) {
        perror("Error when parsing single line code blocks..\n");
        exit(1);
    }

    size_t line_reader_pointer = 0;
    size_t output_writer_pointer = 0;

    while(line_reader_pointer < line_len) {

        if (line_reader_pointer+1 < line_len && line[line_reader_pointer] == '`') {

            char* closing = strchr(&line[line_reader_pointer+1], '`');
            if (closing != NULL) {

                strcpy(&output[output_writer_pointer], "<code class=\"singleline-code-block\">");
                output_writer_pointer += strlen("<code class=\"singleline-code-block\">");
                line_reader_pointer += 1;

                while (&line[line_reader_pointer] != closing) {
                    char one[2] = { line[line_reader_pointer++], '\0' };
                    append_escaped_text(output, output_capacity, &output_writer_pointer, one);
                }

                strcpy(&output[output_writer_pointer], "</code>");
                output_writer_pointer += strlen("</code>");
                line_reader_pointer += 1;
            } else {
                /* An unmatched backtick is literal text; always advance. */
                output[output_writer_pointer++] = line[line_reader_pointer++];
            }

        }
        else {
            //normal chars not in code block
            output[output_writer_pointer++] = line[line_reader_pointer++];
        }
    }

    output[output_writer_pointer] = '\0';
    return output;

}

char* bold_italics_line_giver(char *line) {

    if (line == NULL)
        return NULL;

    size_t line_len = strlen(line);

    if (line_len > (SIZE_MAX - 1) / 8) return NULL;
    char* output = malloc(line_len * 8 + 1);
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
        size_t line_length = strlen(line[i]);
        if (line_length > (SIZE_MAX - 512) / 256 || buffer_size > SIZE_MAX - (line_length * 256 + 512)) {
            fprintf(stderr, "Markdown input is too large to render safely.\n");
            return NULL;
        }
        buffer_size += line_length * 256 + 512;
    }

    // output string in heap so that it survies in main.c
    char* output = malloc((buffer_size+1)*sizeof(char));
    if (output == NULL) {
        perror("Error when making output string...\n");
        exit(1);
    }

    output[0] = '\0';

    //code blocks and lists handled same way
    int in_list = 0;
    int in_code_block = 0;

    //metadata parsing vars
    int metadata_pointer = 0;
    fm.count = 0;

    if (n > 0 && strncmp(line[0], "---", 3) == 0) {

        int closed = 0;

        while (1) {

            metadata_pointer++;

            //bail out instead of reading off the end of line[] if there's no closing ---
            // good safety check added by c-laudey : ill keep it
            if (metadata_pointer >= n) {
                printf("Warning: frontmatter never closed with '---', stopping metadata parse early.\n");
                break;
            }

            if (strncmp(line[metadata_pointer], "---", 3) == 0) {
                closed = 1;
                break;
            }

            //only the first colon separates the key from its value.
            char *keyy = line[metadata_pointer];
            char *valuee = strchr(keyy, ':');
            if (valuee == NULL) {
                printf("Warning: couldn't parse metadata line '%s', skipping it.\n", keyy);
                continue;
            }
            *valuee++ = '\0';
            size_t key_len = strlen(keyy);

            //trimming whitespaces ~ Paper (Quantum Mango)
            while (key_len > 0) {
                size_t i = key_len - 1;
                if (!isspace(keyy[i])) {
                    keyy[i+1] = 0;
                    break;
                }
                key_len--;
            }

            if (strcmp(keyy, "tags") == 0) {
                char *tag_value = valuee;
                while (tag_value != NULL) {
                    char *comma = strchr(tag_value, ',');
                    if (comma != NULL) *comma = '\0';
                    while (isspace((unsigned char)*tag_value)) tag_value++;
                    size_t tag_len = strlen(tag_value);
                    while (tag_len > 0 && isspace((unsigned char)tag_value[tag_len - 1])) tag_value[--tag_len] = '\0';
                    if (tag_len > 0) store_metadata("tag", tag_value);
                    tag_value = comma == NULL ? NULL : comma + 1;
                }
            }
            else {

                while (isspace((unsigned char)*valuee)) valuee++;
                store_metadata(keyy, valuee);

            }

        }

        if (closed) {
            metadata_pointer++; //extra one to skip the ending ---
        }

    }

    for (int i = metadata_pointer; i < n; i++) {

        if (strncmp(line[i], "```", 3) == 0) {
            in_code_block = !in_code_block;
            if (in_code_block) {
                strcat(output, "<div class=\"code-block\"><pre><code>");
                //printf("multiline code block ka strcat of opening tags has finished\n");
            }
            else {
                strcat(output, "</code></pre>\n</div>\n");
            }
            continue; //dont process ``` line
        }

        if (in_code_block) {
            size_t used = strlen(output);
            append_escaped_text(output, buffer_size + 1, &used, line[i]);
            if (used + 1 < buffer_size + 1) {
                output[used++] = '\n';
                output[used] = '\0';
            }
            continue;
        }

        char temp[1024];
        int amount_to_skip_heading = 0;
        int amount_to_skip_list = 0;

        int hash_count = heading_counter(line[i], &amount_to_skip_heading);
        int is_list = list_item_checker(line[i], &amount_to_skip_list);

        if (is_list && !in_list) {
            strcat(output, "<ul>\n");
            in_list = 1;
        }
        else if (!is_list && in_list) {
            strcat(output, "</ul>\n");
            in_list = 0;
        }

        if (is_list) {
            char* bolded_italics_text = bold_italics_line_giver(line[i] + amount_to_skip_list);
            char* linked_text = parse_links(bolded_italics_text);
            char* single_line_codeblocks_text = single_line_codeblock_giver(linked_text);
            char* vid_text = parse_videos(single_line_codeblocks_text);
            char* img_text = parse_images(vid_text);

            if (strncmp(img_text, "[ ] ", 4) == 0) {
                snprintf(temp, sizeof(temp), "<li class=\"checkbox-item\"><input type=\"checkbox\" disabled> %s</li>\n", img_text + 4);
            }
            else if (strncmp(img_text, "[x] ", 4) == 0) {
                snprintf(temp, sizeof(temp), "<li class=\"checkbox-item\"><input type=\"checkbox\" disabled checked> %s</li>\n", img_text + 4);
            }
            else if (strncmp(img_text, ">", 1) == 0 && strlen(img_text) > 0) {
                snprintf(temp, sizeof(temp), "<blockquote class=\"blockquote\"><p>\n%s\n</p></blockquote>", img_text + 2);
            }
            else if (strncmp(img_text, "---", 3) == 0 && strlen(img_text) > 0) {
                snprintf(temp, sizeof(temp), "<hr>\n");
            }
            else {
                snprintf(temp, sizeof(temp), "<li>%s</li>\n", img_text);
            }

            strcat(output, temp);
            free(bolded_italics_text);
            free(linked_text);
            free(single_line_codeblocks_text);
            free(img_text);
            continue;
        }

        char* bolded_italics_text = bold_italics_line_giver(line[i] + amount_to_skip_heading);
        char* linked_text = parse_links(bolded_italics_text);
        char* single_line_codeblocks_text = single_line_codeblock_giver(linked_text);
        char* vid_text = parse_videos(single_line_codeblocks_text);
        char* img_text = parse_images(vid_text);
        free(bolded_italics_text);
        free(linked_text);
        free(single_line_codeblocks_text);

        if (hash_count > 0 && hash_count <= 6 && strlen(img_text) > 0) {
            snprintf(temp, sizeof(temp), "<h%d>%s</h%d>\n", hash_count, img_text, hash_count);
        }
        else if (strncmp(img_text, ">", 1) == 0 && strlen(img_text) > 0) {
            snprintf(temp, sizeof(temp), "<blockquote class=\"blockquote\"><p>\n%s\n</p></blockquote>", img_text + 2);
        }
        else if (strncmp(img_text, "---", 3) == 0 && strlen(img_text) > 0) {
           snprintf(temp, sizeof(temp), "<hr>\n");
        }
        else {
            //if incase we have to include the ** and *
            snprintf(temp, sizeof(temp), "<p>%s</p>\n", img_text);
        }

        strcat(output, temp);
        free(img_text);
    }

    //safety measures
    if (in_code_block) {
        strcat(output, "</code></pre>\n</div>\n");
    }

    if (in_list) {
        strcat(output, "</ul>\n");
    }

    if (output != NULL) {
        return output;
    }
    else {
        return NULL;
    }

}
