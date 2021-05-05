/* A totally overkill program to read a string of arbitrary length from
the standard input file and print it reversed on the standard output file. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// Handy definitions
#define GROW_FACTOR 2
#define INIT_SIZE 8
#define NEW(kind, size) (kind*)reallocate(0, sizeof(kind) * size, NULL)
#define RESIZE(kind, ptr, o, n) (kind*)reallocate(sizeof(kind) * o, sizeof(kind) * n, ptr)
#define FREE(kind, size, ptr) reallocate(sizeof(kind) * size, 0, ptr)

// Since uint_t does not work on Windows, we have to hack support for it somehow
#if defined(_WIN16) || defined(WIN32) || defined(_WIN32) || defined(__WIN32__) || defined(__NT__) || defined(__WINDOWS__) || defined(__TOS_WIN__)
    // This should work with most, if not all, windows compilers
    #define uint_t unsigned long long
#endif


void* reallocate(uint_t oldsize, uint_t newsize, void* ptr) {
    /* Allocates/reallocates/deallocates a block
    of memory using malloc, realloc and free */

    if (oldsize < 0 || newsize < 0) return NULL;
    if (oldsize == 0 && ptr == NULL) { // Allocate from 0 to newsize
        return malloc(newsize);
    }
    else if (newsize == 0 && ptr != NULL) {  // Free object (shrink to 0)
        free(ptr);
        return NULL;
    }
    return realloc(ptr, newsize);   // Realloc handles the other cases!
}


char* reverse(char* str) {
    /* Reverses a string. Returns a copy of the
    string on success and a null pointer on
    failure*/

    uint_t len = strlen(str);  // We use C-strings (null-terminated!)
    char* result = NEW(char, len);
    if (result == NULL) {  // Memory allocation failed
        return NULL;
    }
    memset(result, 0, len);
    for (int i = len - 1; i >= 0; i--) {
        result[len - i - 1] = str[i];
    }
    return result;
}


char* input(char* prompt, bool shrink) {
    /* Reads from stdin until a newline is pressed.
    Returns null upon allocation failure, or a pointer
    to char otherwise.

    Prompt is a message to show to the user.
    Shrink, when set to true, will make sure
    the input buffer size matches the string's length
    exactly, which is less wasteful with memory, but
    also takes more time
    */

    char* buffer = NEW(char, INIT_SIZE);   // We preallocate some space
    if (buffer == NULL) {  // Memory allocation failed
        return NULL;
    }
    memset(buffer, 0, INIT_SIZE * sizeof(char));
    char current;
    uint_t len = 0;
    uint_t buflen = INIT_SIZE * sizeof(char);
    printf("%s", prompt);
    while ((current = getchar()) != '\n') {
        if (len + 1 == buflen) {
            buflen = sizeof(char) * (len + 1) * GROW_FACTOR;
            buffer = RESIZE(char, buffer, len, buflen);
            if (buffer == NULL) {
                return NULL;
            }
        }
        buffer[len] = current;
        len++;
    }
    if (shrink && len < buflen) {
        RESIZE(char, buffer, buflen, len); // We don't waste any bytes!
    }
    buffer[len] = '\0';
    return buffer;
}


int main(int argc, char* argv[]) {
    /* I have no clue what I'm doing */

    char* buffer = input("Type something: ", true);
    char* reversed = reverse(buffer);
    if (buffer == NULL) {
        printf("Error reading input");
        return -1;
    }
    if (reversed == NULL) {
        printf("Error reversing input");
        return -1;
    }
    printf("The reverse of '%s' is '%s'\n", buffer, reversed);
    printf("Length of strings is %lu bytes\n", strlen(buffer));
    FREE(char, strlen(buffer), buffer);
    FREE(char, strlen(reversed), reversed);
}
