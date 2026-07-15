#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "common.h"

#define HTML_BOILERPLATE \
"<!DOCTYPE html>\n" \
"<html lang=\"en\">\n" \
"<head>\n" \
"    <meta charset=\"UTF-8\">\n" \
"    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">\n" \
"    <title>Document</title>\n" \
"    <link rel=\"stylesheet\" href=\"styles.css\">\n" \
"</head>\n" \
"<body>\n" \
"\n" \

/*
 * below is some javascript fuckery to get current tab in the navbar to highlight it
 * i have no idea how it works
 */

#define HTML_BOILERPLATE_ENDING \
"</div>\n" \
"<script>\n" \
"  const currentPath = window.location.pathname.split('/').pop();\n" \
"  document.querySelectorAll('.navbar a').forEach(link => {\n" \
"    const linkPath = link.getAttribute('href');\n" \
"    if (linkPath === currentPath || link.href === window.location.href) {\n" \
"      link.classList.add('active');\n" \
"    }\n" \
"  });\n" \
"</script>\n" \
"</body>\n" \
"</html>"


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

    //building the nav bar
    char navbar_html[8192] = "<nav class=\"navbar\">\n";

    for (int i = 0; i < markdown_names_index; i++) {

        char link_name[128];
        strncpy(link_name, list_of_markdown_names[i], sizeof(link_name)-1);
        link_name[sizeof(link_name)-1] = '\0';

        //removing .md for the href display names thingy
        char* dot = strrchr(link_name, '.');
        if (dot != NULL && strcmp(dot, ".md") == 0)
            *dot = '\0';

        char link_buffer[256];
        snprintf(link_buffer, sizeof(link_buffer), "    <a href=\"%s.html\">%s</a>\n", list_of_markdown_names[i], link_name);
        strcat(navbar_html, link_buffer);

    }

    //all the stuff of the .md will be in this div
    strcat(navbar_html, "</nav>\n<div class=\"content-container\">\n");

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

        char temp[256];
        snprintf(temp, sizeof(temp), "../htmlfiles/%s.html", list_of_markdown_names[i]);
        FILE* fp = fopen(temp, "w");
        if (fp == NULL) {
            perror("Error in creating html file..");
            exit(1);
        }

        char* html_boilerplate = HTML_BOILERPLATE;
        char* html_boilerplate_ending = HTML_BOILERPLATE_ENDING;

        fputs(html_boilerplate, fp);
        fputs(navbar_html, fp);
        fputs(parsed_line, fp);
        fputs(html_boilerplate_ending, fp);

        //cleanup
        fclose(fp);

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
