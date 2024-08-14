# timber

## Intro

This is a personal project, first time writing any sort of library myself. Coming from Python (with one of the best standard library loggers), I was really not satisfied with C's lack of one, so I wrote this really basic debugger library that works relatively similarly to how Python's does.

## Index

[Project Specific Notes](##Notes)

[Example Code](##Examples)

[Configuration](##Configuration)

[Contact Me](##Contact)

## Notes

- This project was compiled with GCC on Linux. Unlike my other project (Mako), this should be cross-platform and cross-compiler.
 
- The log_kill(struct Logger* log) function frees the memory allocated to the Logger as a part of the function. Please do not free this memory yourself as to prevent double frees
 
- Any time a LOG_ERROR occurs, this is your fault, if it's something I've done wrong, the program will crash. Check that your values when calling a function are within the expected ranges

- When writing to a filestream that is not stdout, colors cannot be printed

- All the libraries functions are inline. GNU ld kept having a fit and I don't have that much experience with C, so this is a temporary solution until I can ask my tutors about a better way to go around this
## Examples

The way the library works is simple enough, it includes a new struct type called Logger, as well as various enum types for configuration and function calling

### Basic Usage:

```c
#include <stdio.h>
#include "timber.h"
#include <stdlib.h>

Logger* log;

int main() {
	log = malloc(sizeof(Logger));
	log_init(log, DEBUG, NULL);
	LOG(log, DEBUG, "Example Message");
	log_kill(log);
}
```

### Explanation

1. Globally declare a pointer to a Logger struct. This holds all the configuration data. Global declaration is necessary to ensure the Logger can be used in any function

2. malloc() the correct amount of bytes for the pointer on heap. You could also obviously declare a Logger struct on stack, but since all of the functions take pointers I've written it this way in the example for readability

3. Call the log_init() function, which takes 3 parameters:
	1. Logger*: Pointer to the logger to intialise
	2. enum Levels: The MINIMUM logging level to be printed. This follows the traditional 5 tiered scheme of DEBUG, INFO, WARN, ERROR, CRITICAL
	3. const char*: The filestream to be printed to. **To print to stdout, call log_init() with NULL as the filestream arg**
	log_init() sets defaults for the structs members, and opens the requested filestream

4. Call the LOG() macro as many times as needed to print to the requested filestream. This again takes 3 arguments:
	1. Logger*: Pointer to the logger to print to
	2. enum Levels: The level to print at
	3. const char*: The message to be printed along with debug information
	**NOTE**: The reason this is a macro and not a function is because of the \_\_LINE\_\_ and \_\_func\_\_ constants used in the optional printing process. The real function this calls is \_log\_print()

5. Call log_kill() to free the memory and set the pointer to NULL, and close the attached filestream (unless its stdout).
## Configuration

The Logger struct contains members that control the eventual output of any call to the LOG() macro.

By default, all features are enabled, so if you just want every single bit of relevant information, skip configuring the library.

#### Optional Printable Information
```c
bool time_enabled; //prints the current time
bool line_enabled; //prints the line that the LOG() macro was called from
bool function_enabled; //prints the function that the LOG() macro was called from
Color output_colors[5]; //array of color enum type which corresponds to the debug levels
```

All of these should be edited **AFTER** calling log_init() as to avoid being overwritten.

Since these are all members of a struct, change the values by using the pointer.

As for the colors, the current options are:
- COLOR_GREEN
- COLOR_YELLOW
- COLOR_BLUE
- COLOR_MAGENTA
- COLOR_CYAN
- COLOR_RED
## Contact

If you have any bugs/issues, please contact me via the usual avenues (Tumblr probably) or directly on Github.
