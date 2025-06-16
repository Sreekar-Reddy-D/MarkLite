#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <cctype>
using namespace std;

// Escapes HTML special characters so they render correctly in HTML
string escapeHTML(const string& text) {
    string escaped = text;
    escaped = regex_replace(escaped, regex("&"), "&amp;");
    escaped = regex_replace(escaped, regex("<"), "&lt;");
    escaped = regex_replace(escaped, regex(">"), "&gt;");
    return escaped;
}

// Parses inline markdown elements: bold, italic, links, and images
string parseInline(const string& line) {
    string parsed = line;
    // Bold (**text**)
    parsed = regex_replace(parsed, regex(R"(\*\*(.*?)\*\*)"), "<strong>$1</strong>");
    // Italic (*text*)
    parsed = regex_replace(parsed, regex(R"(\*(.*?)\*)"), "<em>$1</em>");
    // Images (![alt](url))
    parsed = regex_replace(parsed, regex(R"(!\[(.*?)\]\((.*?)\))"), "<img src=\"$2\" alt=\"$1\" />");
    // Links ([text](url))
    parsed = regex_replace(parsed, regex(R"(\[(.*?)\]\((.*?)\))"), "<a href=\"$2\">$1</a>");
    return parsed;
}

// Counts number of leading spaces (used to detect nested lists)
int countLeadingSpaces(const string& s) {
    int count = 0;
    for (char ch : s) {
        if (ch == ' ') count++;
        else break;
    }
    return count;
}

int main() {
    ifstream input("sample.md");      // Markdown input file
    ofstream output("output.html");   // Output HTML file

    // Check if files opened successfully
    if (!input || !output) {
        cerr << "File error.\n";
        return 1;
    }

    // Write HTML document headers
    output << "<!DOCTYPE html>\n<html>\n<head><meta charset=\"UTF-8\"><title>Markdown</title></head>\n<body>\n";

    string line;
    bool insideList = false;      // Track if we're inside a <ul>
    bool insideSubList = false;   // Track if we're inside a nested <ul>
    bool insideCode = false;      // Track if we're inside a code block

    // Process file line-by-line
    while (getline(input, line)) {
        // Start or end of code block
        if (line == "```") {
            output << (insideCode ? "</code></pre>\n" : "<pre><code>\n");
            insideCode = !insideCode;
            continue;
        }

        // If inside code block, write raw escaped HTML and skip further parsing
        if (insideCode) {
            output << escapeHTML(line) << '\n';
            continue;
        }

        // Check for horizontal rule (---)
        if (regex_match(line, regex(R"(\s*---\s*)"))) {
            output << "<hr />\n";
            continue;
        }

        // Detect list item indentation (for nested lists)
        int indent = countLeadingSpaces(line);

        // Match list items starting with `- `
        if (regex_match(line, regex(R"(\s*-\s.+)"))) {
            // Start of a new list
            if (!insideList) {
                output << "<ul>\n";
                insideList = true;
            }

            // Handle nested lists
            if (indent >= 2 && !insideSubList) {
                output << "<ul>\n";
                insideSubList = true;
            } else if (indent < 2 && insideSubList) {
                output << "</ul>\n";
                insideSubList = false;
            }

            // Extract content after the dash and write as a list item
            string item = line.substr(line.find('-') + 1);
            output << "<li>" << parseInline(item) << "</li>\n";
            continue;
        } else {
            // Close any open lists if current line isn't a list
            if (insideSubList) {
                output << "</ul>\n";
                insideSubList = false;
            }
            if (insideList) {
                output << "</ul>\n";
                insideList = false;
            }
        }

        // Match Markdown headings (# to ######)
        if (line.rfind("#", 0) == 0) {
            size_t level = line.find(' '); // Find first space after #
            if (level != string::npos && level <= 6) {
                string content = line.substr(level + 1);
                output << "<h" << level << ">" << parseInline(content) << "</h" << level << ">\n";
                continue;
            }
        }

        // If line is not empty and not a special element, treat as paragraph
        if (!line.empty()) {
            output << "<p>" << parseInline(line) << "</p>\n";
        }
    }

    // Close any unclosed tags at the end
    if (insideCode) output << "</code></pre>\n";
    if (insideSubList) output << "</ul>\n";
    if (insideList) output << "</ul>\n";

    // End of HTML document
    output << "</body>\n</html>";
    input.close();
    output.close();

    cout << "Markdown converted to output.html\n";
    return 0;
}