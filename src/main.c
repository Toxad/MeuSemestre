#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libconfig.h>
#include <ncurses.h>
#include <unistd.h>
#include "header/ui.h"
#include "header/list.h"

int run(config_t*);

int main(int argc, char const *argv[])
{
	// carrega config.cfg, se não achar carrega default.cfg
	config_t cfg;
	config_init(&cfg);

	if(access("config.cfg", F_OK) != -1) {
		config_read_file(&cfg, "config.cfg");
	}
	else if(access("config/default.cfg", F_OK) != -1) {
		config_read_file(&cfg, "config/default.cfg");
	}
	else {
		fprintf(stderr, "ERROR: Not possible to find config file.\n");
		exit(1);
	}

	if(argc > 1) {
		// options
		if( strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0) {
			printf("Usage: program [options]\nOptions:\n\t-h, --help\tdisplay this\n\t-v, --version\tversion of this program\n");
			return 0;
		}
		else if(strcmp(argv[1], "--version") == 0 || strcmp(argv[1], "-v") == 0) {
			config_setting_t* version = config_lookup(&cfg, "version");
			printf("Version: %s\n", (char*) config_setting_get_string(version));
			return 0;
		}
		else {
			fprintf(stderr, "ERROR: Command not recognizable, use --help for help.\n");
			exit(2);
		}
	}
	return run(&cfg);
}

int run(config_t* cfg) {
	initscr();
	cbreak();
	noecho();
	clear();
	curs_set(0);
	keypad(stdscr, TRUE);
	if(LINES < 45 || COLS < 135) {
		printw("ERROR: Minimum number of lines: 45 (%d), columns: 135 (%d)\n", LINES, COLS);
		refresh();
		getch();
		endwin();
		exit(3);
	}
	if(has_colors() == FALSE) {
		printw("Your terminal does not support color\n");
		refresh();
		getch();
		endwin();
		exit(4);
	}

	start_color();
	init_pair(1, COLOR_CYAN, COLOR_BLACK);	// menus
	init_pair(2, COLOR_BLACK, COLOR_WHITE);	// footer
	init_pair(3, COLOR_BLUE, COLOR_BLACK);	// continue
	init_pair(5, COLOR_GREEN, COLOR_BLACK);	// confirm
	init_pair(6, COLOR_RED, COLOR_BLACK);	// exit

	// se não teve um config.cfg, cria um e da set na lingua
	if(access("config.cfg", F_OK) == -1) {
		if(lang_screen(cfg)) {
			config_destroy(cfg);
			endwin();
			return 0;
		}
	}

	unsigned int semester = 0;

	Lista* done_c;
	_LIST_INIT(&done_c);
	config_t all_classes;
	config_init(&all_classes);

	if(course_select_screen(cfg) || 
		done_classes_screen(cfg, done_c, &all_classes)) {

		// caso de saida por esc
		_FREE_LIST(done_c);
		config_destroy(&all_classes);
		config_destroy(cfg);
		endwin();
		return 0;
	}
	while(!result_screen(cfg, done_c, &all_classes, &semester));

	_FREE_LIST(done_c);
	config_destroy(&all_classes);
	config_destroy(cfg);
	endwin();
	
	return 0;
}