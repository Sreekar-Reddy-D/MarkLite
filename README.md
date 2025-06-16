# MarkLite

A simple Markdown to HTML converter in C++, simulating how parsers work.

## Features Supported

- Headings (`#` to `######`)
- Bold and Italic (`**bold**`, `*italic*`)
- Links (`[text](url)`)
- Images (`![alt](image.jpg)`)
- Horizontal rules (`---`)
- Code blocks (```)
- Paragraphs
- Nested lists (1 level)

---

## 🛠 How to Compile and Run

### Using g++:

```bash
g++ converter.cpp -o converter
./converter
````

Make sure `sample.md` exists in the same directory before running the program.
