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
"<footer class=\"site-footer\">\n" \
"    <span class=\"footer-credit\">made with love using <a href=\"https://github.com/NorSomething/c-ssg\" target=\"_blank\">c-ssg</a></span>\n" \
"    <span class=\"footer-links\">\n" \
"        <a href=\"https://github.com/NorSomething\" target=\"_blank\">GitHub</a>\n" \
"        <a href=\"#\">discord: nitmal.</a>\n" \
"        <a href=\"resume.pdf\" target=\"_blank\">Resume</a>\n" \
"    </span>\n" \
"</footer>\n" \
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

//global navbar on every page
char navbar_html[8192] = "<nav class=\"navbar\">\n";

void remove_md_extension(char* out, char* in, size_t out_size) {

    strncpy(out, in, out_size-1);
    out[out_size-1] = '\0';

    char* dot = strchr(out, '.');
    if (dot != NULL && strcmp(dot, ".md") == 0) {
        *dot = '\0';
    }

}

void make_html_page(char* output, char* body) {

    char temp[256];
    snprintf(temp, sizeof(temp), "../htmlfiles/%s.html", output);
    FILE *fp = fopen(temp, "w");
    if (fp == NULL) {
        perror("Error when makign html file..");
        exit(1);
    }

    fputs(HTML_BOILERPLATE, fp);
    fputs(navbar_html, fp);
    fputs(body, fp);
    fputs(HTML_BOILERPLATE_ENDING, fp);
    fclose(fp);

}

void parse_markdown_to_html(char* path, char* output) {

    struct each_file file = get_struct(path);

    if (file.lines == NULL) {
        printf("Read error in %s, skipping...\n", path);
        return;
    }

    char *parsed_line = parse_line(file.lines, file.line_count);

    make_html_page(output, parsed_line);

    //cleanup
    free(parsed_line);
    for (int j = 0; j < file.line_count; j++) {
        free(file.lines[j]);
    }
    free(file.lines);

}

int main() {

    char list_of_markdown_names[127][128];
    int markdown_names_index = 0;

    char list_of_folder_names[32][64];
    int folder_name_index = 0;

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

        //if subfolder remember it
        char full_path[512];
        snprintf(full_path, sizeof(full_path), "../markdowns/%s", de->d_name);

        struct stat st;
        if (stat(full_path, &st) == 0 && S_ISDIR(st.st_mode)) {
            strncpy(list_of_folder_names[folder_name_index], de->d_name, 64-1);
            list_of_markdown_names[folder_name_index][64-1] = '\0';
            folder_name_index++;
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
    for (int i = 0; i < markdown_names_index; i++) {

        char link_name[128];
        remove_md_extension(link_name, list_of_markdown_names[i], sizeof(link_name));

        char link_buffer[256];
        snprintf(link_buffer, sizeof(link_buffer), "    <a href=\"%s.html\">%s</a>\n", list_of_markdown_names[i], link_name);
        strcat(navbar_html, link_buffer);

    }

        //one navbar entry per subfolder
        for (int i = 0; i < folder_name_index; i++) {

            char link_buffer[256];
            snprintf(link_buffer, sizeof(link_buffer), "    <a href=\"%s.html\">%s</a>\n", list_of_folder_names[i], list_of_folder_names[i]);
            strcat(navbar_html, link_buffer);

        }


    //all the stuff of the .md will be in this div
    strcat(navbar_html, "</nav>\n<div class=\"content-container\">\n");

    printf("Found %d markdown files. \n", markdown_names_index);

    //rendering non directory md files
    for (int i = 0; i < markdown_names_index; i++) {
        parse_markdown_to_html(list_of_markdown_names[i], list_of_markdown_names[i]);
    }

    //rendering md files inside a directory
    for (int i = 0; i < folder_name_index; i++) {

        char folder_path[256];
        snprintf(folder_path, sizeof(folder_path), "../markdowns/%s", list_of_folder_names[i]);

        DIR* folder_stream = opendir(folder_path);
        if (folder_stream == NULL) {
            printf("Error when opening folder : %s... skippinh\n", list_of_folder_names[i]);
            continue;
        }

        char file_list_html[4096] = "<ol class=\"file-list\">\n";
        struct dirent *fde; //file directory entry

        while((fde = readdir(folder_stream)) != NULL) {

            if ((strcmp(fde->d_name, ".") == 0) ||  (strcmp(fde->d_name, "..") == 0)) {
                continue;
            }

            size_t len = strlen(fde->d_name);

            //same getting md file logic from dir as above
            if (len < 3 || (strcmp(fde->d_name + len - 2, "md") != 0)) {
                continue;
            }

            char link_name[128];
            remove_md_extension(link_name, fde->d_name, sizeof(link_name));

            char name_of_html[256];
            snprintf(name_of_html, sizeof(name_of_html), "%s__%s", list_of_folder_names[i], link_name);

            char dir_list_buffer[512];
            snprintf(dir_list_buffer, sizeof(dir_list_buffer), "<li><a href=\"%s.html\">%s</a></li>\n", name_of_html, link_name);

            char sub_path[512];
            snprintf(sub_path, sizeof(sub_path), "%s/%s", list_of_folder_names[i], fde->d_name);
            strcat(file_list_html, dir_list_buffer);

            parse_markdown_to_html(sub_path, name_of_html);

        }

        closedir(folder_stream);
        strcat(file_list_html, "</ol>\n");

        make_html_page(list_of_folder_names[i], file_list_html);

    }

    return 0;

}
