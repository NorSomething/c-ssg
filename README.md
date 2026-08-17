
# c-ssg

A *very opinionated* and minimal static site generator written in C.

`c-ssg` takes a directory of Markdown files and generates a complete static website with a shared navigation bar, nested sections, and plain HTML output. It is super lightweight and built using standard POSIX interfaces.

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/d16579b9-edea-48fb-8806-a307e28522c7" />


<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/99d75776-a5b0-42de-b088-aa11781a5084" />









---

## Features

- Markdown to HTML conversion
- Automatic navigation bar generation
- Image support
- Video support
- Nested documentation support through folders
- Shared CSS across every generated page
- Bottom bar
- Front matter metadata support
- Lightmode/Darkmode Theme Switcher

---

## Project Structure

```
.
├── src/                # Source code
├── site-imgs/          # Your site images
    ├── img1.png
    ├── img2.png
├── markdowns/          # Your markdown files
│   ├── index.md
│   ├── about.md
│   └── guides/
│       ├── install.md
│       └── usage.md
├── cssfiles/           # Your global css files
    ├── styles.css
├── htmlfiles/          # Generated HTML and CSS
│   ├── index.html
│   ├── about.html
│   ├── guides.html
│   ├── guides__install.html
│   └── guides__usage.html
```

---

## How it Works

Every Markdown file placed directly inside `markdowns/` becomes its own page.

For example,

```
markdowns/
├── index.md
├── about.md
└── contact.md
```

generates

```
Home | About | Contact
```

in the navigation bar.

Folders inside `markdowns/` become navigation entries themselves.

For example,

```
markdowns/
├── index.md
└── guides/
    ├── installation.md
    ├── configuration.md
    └── deployment.md
```

generates a navigation bar like

```
Home | Guides
```

Clicking **Guides** opens a page containing links to

- Installation
- Configuration
- Deployment

Each Markdown file inside the folder is also converted into its own HTML page.

### Images
To insert images into any of the websites, you will have to store the image in the `site-imgs` directory with a unique name and insert the image in the markdown file in the following format : 
```
~img=<image name>~
```
For example, to insert an image of a cat, file name being `cat.png`, edit the markdown file to contain:
```
~img=cat.png
```
Save the `cat.png` file in the `site-imgs` directory.

### Videos
Videos insertion follow the same syntax as images.
```
~vid=cat.mp4~
```
Save the `cat.mp4` file in the `site-imgs` directory as well.

### Metadata 
To change the metadata of every html page, such as title, date, etc., the following frontmatter metadata format should be used in the markdown files.
```
---
title: <pagetitle>
tags: <tag1>, <tag2>, <tag3>
---
Your markdown content
```
As of now, there is only support for the website's title tag and tags for the markdown files inside the nested directory in `/markdowns`, more keys are planned in the future.

### Note
To change the bottom bar, edit the `main.c` file's `#define HTML_BOILERPLATE_ENDING` to change or include whatever footer elements you might need.

---

## Supported Markdown

Current support includes:

- Headings (`#` through `######`)
- Paragraphs
- Unordered lists
- **Bold**
- *Italic*
- ```code blocks```
- [Links](https://www.youtube.com/watch?v=dQw4w9WgXcQ)
- [x] Checkboxes
- > Block Quotes
- Line Breaks

More syntax is planned.

---

## Building

Build using the included makefile.

```bash
make
```

Run the generator:

```bash
./c-ssg
```

Generated files will appear inside `htmlfiles/`.

---

## Customization

The generated pages all share the same stylesheet.

```
cssfiles/styles.css
```

Editing this file changes the appearance of the entire website.

---

## Todo

- [x] Code blocks
- [x] Single line code blocks 
- [ ] Syntax Highlighting (metadata option)
- [x] Images
- [x] Page titles
- [ ] More metadata support
- [ ] Searchable and filterable tags (from frontmatter)
- [ ] Extend tags frontmatter to different metadata keys.
- [x] Checkboxes
- [x] Links
- [x] Blockquotes
- [ ] Ordered lists
- [ ] Tables
- [x] Better Markdown compatibility (in list markdown)

---

## License

MIT
