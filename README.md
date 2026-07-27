
# c-ssg

A minimal static site generator written in C.

`c-ssg` takes a directory of Markdown files and generates a complete static website with a shared navigation bar, nested sections, and plain HTML output. It is super lightweight and built using standard POSIX interfaces.

<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/aab237a6-2167-4251-aaea-aa29ce3d4ad9" />
<img width="1920" height="1080" alt="image" src="https://github.com/user-attachments/assets/ee9339c6-bb1c-44db-b37f-c65cc9e60f81" />





---

## Features

- Markdown to HTML conversion
- Automatic navigation bar generation
- Nested documentation support through folders
- Shared CSS across every generated page
- Bottom bar

---

## Project Structure

```
.
├── src/                # Source code
├── markdowns/          # Your markdown files
│   ├── index.md
│   ├── about.md
│   └── guides/
│       ├── install.md
│       └── usage.md
├── htmlfiles/          # Generated HTML and CSS
│   ├── styles.css
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

More syntax is planned.

---

## Building

Compile the source files with GCC.

```bash
cd src
gcc *.c -o c-ssg
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
htmlfiles/styles.css
```

Editing this file changes the appearance of the entire website.

---

## Todo

- [ ] Code blocks
- [ ] Images
- [ ] Page titles
- [ ] Links
- [ ] Blockquotes
- [ ] Ordered lists
- [ ] Tables
- [ ] Better Markdown compatibility

---

## License

MIT
