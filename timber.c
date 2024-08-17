#include "timber.h"
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// DESC: Returns the ANSI color code of the color type
// NOTE: this was implemented to make editing the logger configuration less confusing on the user end. Future Charlotte: yes, i do in fact know that strncpy exists
static const char *_get_color_code(color_t color) 
{
	switch (color) {
		case COLOR_GREEN: return "\033[32m";
		case COLOR_YELLOW: return "\x1b[33m";
		case COLOR_BLUE: return "\x1b[34m";
		case COLOR_MAGENTA: return "\x1b[35m";
		case COLOR_CYAN: return "\x1b[36m";
		case COLOR_RED: return "\033[31m";
		case ANSI_RESET: return "\033[0m";
		default: return NULL; // if color isnt known, return NULL for later error handling
	}

}

// DESC: _log_err() is called whenever there is a USER error.
static void _log_err(const char *msg)
{
	fprintf(stderr, "[LOG ERROR]: %s\n", msg);
}

// NOTE: Function must be called before you log
// DESC: Sets up the logger, opens the file and validates the inputs are actually usable
void log_init(logger_t *log, const enum Levels min_level, const char *file_name)
{
  if (log == NULL) {
    _log_err("NULL Logger Pointer");
    exit(1);
  }
	//Initialising Defaults
	log->time_enabled = true;
	log->line_enabled = true;
	log->function_enabled = true;
	
	//Default output colors
	log->output_colors[0] = COLOR_GREEN;
	log->output_colors[1] = COLOR_CYAN;
	log->output_colors[2] = COLOR_YELLOW;
	log->output_colors[3] = COLOR_RED;
	log->output_colors[4] = COLOR_RED;

	// Open requested file or write to stdout
	if (file_name!=NULL) {
		log->stream = fopen(file_name, "w");
	} else {
		log->stream = stdout;
	}

	if (min_level>4 || min_level<0) {
		_log_err("Invalid Minimum Log Level");
		log->min_level = 1;
	} else {
		log->min_level = min_level;
	}
}

// DESC: Kill requested logger
void log_kill(logger_t *log)
{
	if (log->stream != NULL) {fclose(log->stream);}
  free(log);
	log = NULL; // set pointer to NULL
}

// NOTE: This function uses a macro to simplify calling. args "function" and "line" should not be input manually by the user
// DESC: Produces and prints a formatted output using the configuration from the logger struct.
void _log_print(const char *function, const int line,
                logger_t *log, const enum Levels level, const char *msg)
{
	const char *levels[] = {"DEBUG", "INFO", "WARNING", "ERROR", "CRITICAL"};
	time_t now = time(NULL);
	//Handling of Errors
	if (log == NULL) {
    _log_err("NULL Logger pointer");
    return;
  }
	if (level>4 || level<0) {
    _log_err("Invalid Log Level");
    return;
  }
	if (_get_color_code(log->output_colors[level]) == NULL) {
    _log_err("Invalid Color");
    return;
  }
	
	// Form the final output based off of which toggles in the struct are enabled
	char sn_buffer[128];
	char optional_output[256] = ""; // intialise the output, apparently strcat doesnt like empty strings


	if (log->time_enabled) {
		snprintf(sn_buffer, sizeof(sn_buffer), "%s", ctime(&now));
		strcat(optional_output, sn_buffer);
	}
	if (log->function_enabled) {
		snprintf(sn_buffer, sizeof(sn_buffer), "In Function: %s() ", function);
		strcat(optional_output, sn_buffer);
	}
	if (log->line_enabled) {
		snprintf(sn_buffer, sizeof(sn_buffer), "At Line: %d", line);
		strcat(optional_output, sn_buffer);
	}
  // Finally, check that the level is greater than or equal to the minimum printing level
  if (level < log->min_level) {
    return;
  }
  
  // Don't print escape codes if the output is anything but stdout
  if (log->stream != stdout) {
    fprintf(log->stream, "%s [%s]: %s\n", optional_output, levels[level], msg);
  } else {
    fprintf(log->stream, "%s%s [%s]: %s\n", _get_color_code(log->output_colors[level]), optional_output, levels[level], msg);
    //Reset colors 
    fprintf(log->stream, "%s", _get_color_code(ANSI_RESET));
  }
}
