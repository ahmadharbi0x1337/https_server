#include <stdio.h>
#include <string.h>
typedef struct
{
    const char* extension;
    const char* mime_type;
}MimeType;


// Define MIME Types
static const MimeType mime_types[] = {
    {".html", "text/html"},
    {".css", "text/css"},
    {".js", "application/javascript"},
    {".png", "image/png"},
    {".jpg", "image/jpg"},
    {".gif", "image/gif"},
    {".txt", "text/plain"},
    {NULL, NULL} // Sentinel Value
};


// Function To Get Mime Type By Extension
const char* get_mime_type(const char* filename)
{
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) return "application/octet-stream"; // Default MIME type

    // Check Each MIME type
    for (int i = 0; mime_types[i].extension != NULL; i++)
    {
        if (strcmp(dot, mime_types[i].extension) == 0)
        {
            return mime_types[i].mime_type;
        }
    }
    return "application/octet-stream";
}