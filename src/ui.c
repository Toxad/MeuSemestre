#include "header/ui.h"
#include "header/list.h"
#include <ncurses.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <libconfig.h>

/* Constantes */

#define BOX_HEIGHT 			24
#define BOX_WIDTH			80
#define MAX_LANG_SIZE 		20
#define KEY_ESC 			27
#define _KEY_ENTER			10
#define LANG_SCREEN_W		30
#define COURSE_SCREEN_W		35
#define CONFIRM_SCREEN_W	45
#define KEY_SPACE			' '

/* Referencia para os metodos posteriormente implementados */

int confirm_screen(const char*);

int continue_screen();

int exit_screen();

void print_footer();

void print_exit(WINDOW*,
				unsigned int);

void print_menu(WINDOW*,
				unsigned int,
				Lista*);

void print_confirm(WINDOW*,
					unsigned int,
					const char*);

void print_done_classes(WINDOW*,
						unsigned int,
						unsigned int,
						unsigned int,
						Lista*,
						Lista*);

void print_result(WINDOW*,
					unsigned int,
					unsigned int,
					Lista*,
					Lista*);

void print_continue(WINDOW*,
					unsigned int);

int cmp_settings(void*, void*);

int cmp_settings_strv(void*, void*);

char* trim_it(const char*);

Lista* calc_poss(Lista*, config_t*);


/* Implementação */

// tela de seleção de linguagem
int lang_screen(config_t* cfg) {
	// pegar as escolhas aqui
	config_setting_t* lang_options_sett = config_lookup(cfg, "lang");
	unsigned int count = config_setting_length(lang_options_sett);

	WINDOW *menu_win;
	Lista* lista_langs;
	_LIST_INIT(&lista_langs);

	unsigned int i;
	for(i = 0; i < count; i++) {
		config_setting_t* elem = config_setting_get_elem(lang_options_sett, i);
		const char* str;
		config_setting_lookup_string(elem, "title", &str);
		_INSERT_LIST(lista_langs, (void*) str);
	}

	int startx = (COLS - LANG_SCREEN_W) / 2;
	int starty = (LINES - _GET_SIZE(lista_langs)+3) / 2;
	menu_win = newwin(_GET_SIZE(lista_langs)+4, LANG_SCREEN_W, starty, startx);
	
	keypad(menu_win, TRUE);

	unsigned int highlight = 0;
	wattron(menu_win, COLOR_PAIR(1));

	print_footer();
	print_menu(menu_win, highlight, lista_langs);
	wrefresh(menu_win);
	int c;
	while(1) {
		c = wgetch(menu_win);
		switch(c) {
			case KEY_DOWN:
				highlight = (++highlight) % _GET_SIZE(lista_langs);
				break;
			case KEY_UP:
				if(highlight == 0) highlight = _GET_SIZE(lista_langs)-1;
				else highlight--;
				break;
			case KEY_ESC:
				if(exit_screen()) {
					wclear(menu_win);
					wrefresh(menu_win);
					delwin(menu_win);
					_FREE_LIST(lista_langs);
					return 1; // sucesso, porem saindo
				}
				break;
			case _KEY_ENTER:
				if(confirm_screen((char*) _GET_ELEM_LIST(lista_langs, highlight))) {
					// elemento representante do novo lang
					config_setting_t* elem = config_setting_get_elem(lang_options_sett, highlight);

					// strings
					const char* lang_title;
					const char* lang_file;

					// suas strings
					config_setting_lookup_string(elem, "title", &lang_title);
					config_setting_lookup_string(elem, "file", &lang_file);

					// configs
					config_setting_t* title_elem = config_lookup(cfg, "default_lang");
					config_setting_t* file_elem = config_lookup(cfg, "default_lang_path");

					// set new strings
					config_setting_set_string(title_elem, lang_title);
					config_setting_set_string(file_elem, lang_file);

					config_write_file(cfg, "config.cfg");
					wclear(menu_win);
					wrefresh(menu_win);
					delwin(menu_win);
					_FREE_LIST(lista_langs);
					return 0; // sucesso
				}
				break;
		}
		print_menu(menu_win, highlight, lista_langs);
		print_footer();
	}
}

// tela de seleção de curso
int course_select_screen(config_t* cfg) {
	config_setting_t* course_options_sett = config_lookup(cfg, "course");
	unsigned int count = config_setting_length(course_options_sett);

	WINDOW *menu_win;
	Lista* lista_curso;
	_LIST_INIT(&lista_curso);
	unsigned int i;
	for(i = 0; i < count; i++) {
		config_setting_t* elem = config_setting_get_elem(course_options_sett, i);
		const char* str;
		config_setting_lookup_string(elem, "title", &str);
		_INSERT_LIST(lista_curso, (void*) str);
	}

	int startx = (COLS - COURSE_SCREEN_W) / 2;
	int starty = (LINES - _GET_SIZE(lista_curso) + 3) / 2;
	menu_win = newwin(_GET_SIZE(lista_curso) + 4, LANG_SCREEN_W, starty, startx);
	
	keypad(menu_win, TRUE);

	unsigned int highlight = 0;
	wattron(menu_win, COLOR_PAIR(1));

	print_footer();
	print_menu(menu_win, highlight, lista_curso);
	wrefresh(menu_win);
	int c;
	while(1) {
		c = wgetch(menu_win);
		switch(c) {
			case KEY_DOWN:
				highlight = (++highlight) % _GET_SIZE(lista_curso);
				break;
			case KEY_UP:
				if(highlight == 0) highlight = _GET_SIZE(lista_curso) - 1;
				else highlight--;
				break;
			case KEY_ESC:
				if(exit_screen()) {
					wclear(menu_win);
					wrefresh(menu_win);
					delwin(menu_win);
					_FREE_LIST(lista_curso);
					return 1; // sucesso, porem saindo
				}
				break;
			case _KEY_ENTER:
				if(confirm_screen((char*) _GET_ELEM_LIST(lista_curso, highlight))) {
					// fazer as parada aqui
					// problema aqui
					config_setting_t* elem = config_setting_get_elem(course_options_sett, highlight);

					const char* course_title;
					const char* course_file;

					config_setting_lookup_string(elem, "title", &course_title);
					config_setting_lookup_string(elem, "file", &course_file);

					config_setting_t* root = config_root_setting(cfg);

					config_setting_t* title_elem = config_setting_add(root, "actual_course", CONFIG_TYPE_STRING);
					config_setting_t* file_elem = config_setting_add(root, "actual_course_path", CONFIG_TYPE_STRING);

					config_setting_set_string(title_elem, course_title);
					config_setting_set_string(file_elem, course_file);

					wclear(menu_win);
					wrefresh(menu_win);
					delwin(menu_win);
					_FREE_LIST(lista_curso);
					return 0; // sucesso
				}
				break;
		}
		print_menu(menu_win, highlight, lista_curso);
		print_footer();
	}
}

// print da tela de menu (todos)
void print_menu(WINDOW *menu_win, unsigned int highlight, Lista* l) {
	int x = 2, y = 2, i;
	box(menu_win, 0, 0);
	for(i = 0; i < _GET_SIZE(l); i++) {
		if(highlight == i) {
			wattron(menu_win, A_REVERSE);
			mvwprintw(menu_win, y, x, "%s", (char*) _GET_ELEM_LIST(l, i));
			wattroff(menu_win, A_REVERSE);
		}
		else
			mvwprintw(menu_win, y, x, "%s", (char*) _GET_ELEM_LIST(l, i));
		++y;
	}
	wrefresh(menu_win);
}

// tela de selecionar materias ja cursadas
int done_classes_screen(config_t* cfg, Lista* done_c, config_t* all_classes) {
	Lista* lista_materias;
	_LIST_INIT(&lista_materias);

	config_setting_t* path = config_lookup(cfg, "actual_course_path");
	const char* filename = config_setting_get_string(path);
	char complete_path[80] = "config/course/";
	strcat(complete_path, filename);
	config_read_file(all_classes, complete_path);

	config_setting_t* classes_set = config_lookup(all_classes, "classes");
	unsigned int count_classes = config_setting_length(classes_set);
	unsigned int i;

	for(i = 0; i < count_classes; i++) {
		config_setting_t* elem = config_setting_get_elem(classes_set, i);
		_INSERT_LIST(lista_materias, (void*) elem);
	}

	unsigned int max_lines = LINES-10;
	WINDOW *done_c_win;
	int startx = 2;
	int starty = 2;
	done_c_win = newwin(max_lines, COLS-4, starty, startx);
	keypad(done_c_win, TRUE);

	unsigned int highlight = 0;
	wattron(done_c_win, COLOR_PAIR(1));

	print_footer();
	print_done_classes(done_c_win, highlight, count_classes, max_lines, lista_materias, done_c);
	refresh();
	int c;
	while(1) {
		c = wgetch(done_c_win);
		switch(c) {
			case KEY_DOWN:
				highlight = (++highlight) % _GET_SIZE(lista_materias);
				break;
			case KEY_UP:
				if(highlight == 0) highlight = _GET_SIZE(lista_materias) - 1;
				else highlight--;
				break;
			case KEY_ESC:
				// exit
				if(exit_screen()) {
					wclear(done_c_win);
					wrefresh(done_c_win);
					delwin(done_c_win);
					_FREE_LIST(lista_materias);
					return 1; // sucesso, porem saindo
				}
				break;
			case KEY_SPACE:
				// se não tiver dentro ja, coloca-o
				if(!_IS_INSIDE(_GET_ELEM_LIST(lista_materias, highlight), done_c, cmp_settings)) {
					_INSERT_LIST(done_c, _GET_ELEM_LIST(lista_materias, highlight));
					break;
				}
				break;
			case _KEY_ENTER:
				// next screen
				if(confirm_screen("Already done classes")) {
					wclear(done_c_win);
					wrefresh(done_c_win);
					delwin(done_c_win);
					_FREE_LIST(lista_materias);
					return 0;
				}
				break;	
		}
		print_done_classes(done_c_win, highlight, count_classes, max_lines, lista_materias, done_c);
		print_footer();
	}
}

// print da tela de selecionar materias ja cursadas
void print_done_classes(WINDOW* done_c_win, unsigned int highlight,	unsigned int count_classes, unsigned int max_lines, Lista* lista_materias, Lista* done_c) {

	max_lines -= 3; // 1 pula linha, 2 pelas paredes
	int x = 2, y = 2, i;
	unsigned int attributes = 0;
	wclear(done_c_win);
	box(done_c_win, 0, 0);

	if((count_classes - highlight) > max_lines) {
		for(i = highlight; i < max_lines+highlight; i++) {
			if(highlight == i) {
				wattron(done_c_win, A_REVERSE );
				attributes += 1;
			}
			if(_IS_INSIDE(_GET_ELEM_LIST(lista_materias, i), done_c, cmp_settings)) {
				wattron(done_c_win, A_BOLD);
				attributes += 2;
			}
			config_setting_t* elem = _GET_ELEM_LIST(lista_materias, i);
			elem = config_setting_get_member(elem, "title");
			mvwprintw(done_c_win, y, x, "%s", config_setting_get_string(elem));
			switch(attributes) {
				case 3:
					wattroff(done_c_win, A_BOLD);
				case 1:
					wattroff(done_c_win, A_REVERSE );
					break;
				case 2:
					wattroff(done_c_win, A_BOLD);
					break;
			}
			attributes = 0;
			++y;
		}
	}
	else {
		for(i = count_classes-max_lines; i < count_classes; i++) {
			if(highlight == i) {
				wattron(done_c_win, A_REVERSE );
				attributes += 1;
			}
			if(_IS_INSIDE(_GET_ELEM_LIST(lista_materias, i), done_c, cmp_settings)) {
				wattron(done_c_win, A_BOLD);
				attributes += 2;
			}
			config_setting_t* elem = _GET_ELEM_LIST(lista_materias, i);
			elem = config_setting_get_member(elem, "title");
			mvwprintw(done_c_win, y, x, "%s", config_setting_get_string(elem));
			switch(attributes) {
				case 3:
					wattroff(done_c_win, A_BOLD);
				case 1:
					wattroff(done_c_win, A_REVERSE );
					break;
				case 2:
					wattroff(done_c_win, A_BOLD);
					break;
			}
			attributes = 0;
			++y;
		}
	}
	wrefresh(done_c_win);
}

// tela de resultado
int result_screen(config_t* cfg, Lista* done_c, config_t* all_classes, unsigned int* sem_count) {
	(*sem_count)++;

	unsigned int max_lines = LINES-13;
	WINDOW *finish_win;
	int startx = 2;
	int starty = 5;
	finish_win = newwin(max_lines, COLS-4, starty, startx); // maximo
	keypad(finish_win, TRUE);

	Lista* poss_list = calc_poss(done_c, all_classes);

	attron(A_BOLD);
	mvprintw(2, 2, "You can take these classes this (%d) semester:", *sem_count);
	attroff(A_BOLD);
	refresh();
	
	unsigned int highlight = 0;
	wattron(finish_win, COLOR_PAIR(1));

	print_footer();
	print_result(finish_win, highlight, max_lines, done_c, poss_list);
	int c;
	while(1) {
		c = wgetch(finish_win);
		switch(c) {
			case KEY_DOWN:
				highlight = (++highlight) % poss_list->tamanho;
				break;
			case KEY_UP:
				if(highlight == 0) highlight = poss_list->tamanho-1;
				else highlight--;
				break;
			case KEY_ESC:
				// exit
				if(exit_screen()) {
					wclear(finish_win);
					wrefresh(finish_win);
					delwin(finish_win);
					_FREE_LIST(poss_list);
					return 1; // sucesso, porem saindo
				}
				break;
			case KEY_SPACE:
				// se não tiver dentro ja, coloca-o

				if(!_IS_INSIDE(_GET_ELEM_LIST(poss_list, highlight), done_c, cmp_settings)) {
					_INSERT_LIST(done_c, _GET_ELEM_LIST(poss_list, highlight));
					break;
				}
				break;
			case _KEY_ENTER:
				// next screen
				if(continue_screen()) {
					wclear(finish_win);
					wrefresh(finish_win);
					delwin(finish_win);
					_FREE_LIST(poss_list);
					return 0;
				}
				break;
		}
		print_result(finish_win, highlight, max_lines, done_c, poss_list);
		print_footer();
	}
}

void print_result(WINDOW* finish_win, unsigned int highlight, unsigned int max_lines, Lista* done_c, Lista* poss_list) {

	max_lines -= 3; // 1 pula linha, 2 pelas paredes
	int x = 2, y = 2, i;
	unsigned int attributes = 0;
	wclear(finish_win);
	box(finish_win, 0, 0);

	if((poss_list->tamanho - highlight) > max_lines) {
		for(i = highlight; i < max_lines+highlight; i++) {
			if(highlight == i) {
				wattron(finish_win, A_REVERSE );
				attributes += 1;
			}
			if(_IS_INSIDE(_GET_ELEM_LIST(poss_list, i), done_c, cmp_settings)) {
				wattron(finish_win, A_BOLD);
				attributes += 2;
			}
			config_setting_t* elem = _GET_ELEM_LIST(poss_list, i);
			elem = config_setting_get_member(elem, "title");
			mvwprintw(finish_win, y, x, "%s", config_setting_get_string(elem));
			switch(attributes) {
				case 3:
					wattroff(finish_win, A_BOLD);
				case 1:
					wattroff(finish_win, A_REVERSE );
					break;
				case 2:
					wattroff(finish_win, A_BOLD);
					break;
			}
			attributes = 0;
			++y;
		}
	}
	else {
		if(poss_list->tamanho < max_lines)
			i = 0;
		else
			i = poss_list->tamanho-max_lines;
		for(; i < poss_list->tamanho; i++) {
			if(highlight == i) {
				wattron(finish_win, A_REVERSE );
				attributes += 1;
			}
			if(_IS_INSIDE(_GET_ELEM_LIST(poss_list, i), done_c, cmp_settings)) {
				wattron(finish_win, A_BOLD);
				attributes += 2;
			}
			config_setting_t* elem = _GET_ELEM_LIST(poss_list, i);
			elem = config_setting_get_member(elem, "title");
			mvwprintw(finish_win, y, x, "%s", config_setting_get_string(elem));
			switch(attributes) {
				case 3:
					wattroff(finish_win, A_BOLD);
				case 1:
					wattroff(finish_win, A_REVERSE );
					break;
				case 2:
					wattroff(finish_win, A_BOLD);
					break;
			}
			attributes = 0;
			++y;
		}
	}
	wrefresh(finish_win);
}

// calcula todas as materias que podem ser pegas no semestre com base em uma lista (pilha nesse caso)
Lista* calc_poss(Lista* done_c, config_t* all_classes) {
	// init perm

	Lista* lista_poss;
	_LIST_INIT(&lista_poss);

	config_setting_t* classes = config_lookup(all_classes, "classes");
	unsigned int count = config_setting_length(classes), i, j;
	int inside = 1;
	for(i = 0; i < count; i++) {
		config_setting_t* elem = config_setting_get_elem(classes, i);
		config_setting_t* arr = config_setting_get_member(elem, "req");
		config_setting_t* cod_ = config_setting_get_member(elem, "cod");
		const char* cod_str = config_setting_get_string(cod_);
		int req_count = config_setting_length(arr);

		if(_IS_INSIDE((void*) cod_str, done_c, cmp_settings_strv))
			continue;

		for(j = 0; j < req_count; j++) {
			const char* str = config_setting_get_string_elem(arr, j);
			if(!_IS_INSIDE((void*)str, done_c, cmp_settings_strv))
				inside = 0;
		}
		if(inside)
			_INSERT_LIST(lista_poss, elem);
		inside = 1;
	}
	return lista_poss;
}

// tela de confirmação
int confirm_screen(const char* msg) {
	WINDOW *confirm_win;
	int startx = (COLS - CONFIRM_SCREEN_W) / 2;
	int starty = (LINES - 6) / 2;
	confirm_win = newwin(6, CONFIRM_SCREEN_W, starty, startx);
	keypad(confirm_win, TRUE);
	wattron(confirm_win, COLOR_PAIR(5));

	int highlight = 0;
	print_confirm(confirm_win, highlight, msg);
	int c;
	while(1) {
		c = wgetch(confirm_win);
		switch(c) {
			case KEY_LEFT:
				highlight = (++highlight) % 2;
				break;
			case KEY_RIGHT:
				highlight = (++highlight) % 2;
				break;
			case _KEY_ENTER:
				wclear(confirm_win);
				wrefresh(confirm_win);
				delwin(confirm_win);
				if(highlight == 0)
					return 1;
				return 0;
		}
		print_confirm(confirm_win, highlight, msg);
	}
	return 1;
}

// print da tela de confirmação
void print_confirm(WINDOW* confirm_win, unsigned int highlight, const char* msg) {
	int x = 2, y = 2;
	box(confirm_win, 0, 0);
	mvwprintw(confirm_win, y++, x, "You've selected: %s", msg);
	if(highlight) {
		mvwprintw(confirm_win, y, x, "Confirm");
		wattron(confirm_win, A_REVERSE);
		mvwprintw(confirm_win, y++, x+18, "Cancel");
		wattroff(confirm_win, A_REVERSE);
	}
	else {
		wattron(confirm_win, A_REVERSE);
		mvwprintw(confirm_win, y, x, "Confirm");
		wattroff(confirm_win, A_REVERSE);
		mvwprintw(confirm_win, y++, x+18, "Cancel");
	}
	wrefresh(confirm_win);
}

// tela de continuação
int continue_screen() {
	WINDOW *continue_win;
	int startx = (COLS - CONFIRM_SCREEN_W) / 2;
	int starty = (LINES - 6) / 2;
	continue_win = newwin(6, CONFIRM_SCREEN_W, starty, startx);
	keypad(continue_win, TRUE);
	wattron(continue_win, COLOR_PAIR(3));

	unsigned int highlight = 0;
	print_continue(continue_win, highlight);
	int c;
	while(1) {
		c = wgetch(continue_win);
		switch(c) {
			case KEY_LEFT:
			case KEY_RIGHT:
				highlight = (++highlight) % 2;
				break;
			case _KEY_ENTER:
				wclear(continue_win);
				wrefresh(continue_win);
				delwin(continue_win);
				if(highlight == 0)
					return 1;
				return 0;
		}
		print_continue(continue_win, highlight);
	}
	return 1;
}

// print tela de continuação
void print_continue(WINDOW* continue_win, unsigned int highlight) {
	int x = 2, y = 2;
	box(continue_win, 0, 0);
	mvwprintw(continue_win, y++, x, "You want to continue?");
	if(highlight) {
		mvwprintw(continue_win, y, x, "Confirm");
		wattron(continue_win, A_REVERSE);
		mvwprintw(continue_win, y++, x+18, "Cancel");
		wattroff(continue_win, A_REVERSE);
	}
	else {
		wattron(continue_win, A_REVERSE);
		mvwprintw(continue_win, y, x, "Confirm");
		wattroff(continue_win, A_REVERSE);
		mvwprintw(continue_win, y++, x+18, "Cancel");
	}
	wrefresh(continue_win);
}

// tela de saida
int exit_screen() {
	WINDOW *exit_win;
	int startx = (COLS - CONFIRM_SCREEN_W) / 2;
	int starty = (LINES - 6) / 2;
	exit_win = newwin(6, CONFIRM_SCREEN_W, starty, startx);
	keypad(exit_win, TRUE);
	wattron(exit_win, COLOR_PAIR(6));

	int highlight = 0;
	print_exit(exit_win, highlight);
	int c;
	while(1) {
		c = wgetch(exit_win);
		switch(c) {
			case KEY_LEFT:
				highlight = (++highlight) % 2;
				break;
			case KEY_RIGHT:
				highlight = (++highlight) % 2;
				break;
			case _KEY_ENTER:
				wclear(exit_win);
				wrefresh(exit_win);
				delwin(exit_win);
				if(highlight == 0)
					return 1;
				return 0;
		}
		print_exit(exit_win, highlight);
	}
	return 1;
}

// print da tela de saida
void print_exit(WINDOW* exit_win, unsigned int highlight) {
	int x = 2, y = 2;
	box(exit_win, 0, 0);
	mvwprintw(exit_win, y++, x, "Exit?");
	if(highlight) {
		mvwprintw(exit_win, y, x, "Confirm");
		wattron(exit_win, A_REVERSE);
		mvwprintw(exit_win, y++, x+15, "Cancel");
		wattroff(exit_win, A_REVERSE);
	}
	else {
		wattron(exit_win, A_REVERSE);
		mvwprintw(exit_win, y, x, "Confirm");
		wattroff(exit_win, A_REVERSE);
		mvwprintw(exit_win, y++, x+15, "Cancel");
	}
	wrefresh(exit_win);
}

// barra de ajuda na parte inferior da tela
void print_footer() {
	attron(COLOR_PAIR(2));
	mvprintw(LINES-1, 0, "KEYS: UP, DOWN, LEFT, RIGHT, RETURN (confirm), ESC (exit), SPACE (select)");
	refresh();
	attroff(COLOR_PAIR(2));
}

// função para comparar dois elementos settings (pela memoria)
int cmp_settings(void *a, void *b) {

	if(&(*(config_setting_t*)a) == &(*(config_setting_t*)b))
		return 1;
	return 0;
}

// deixa somente caracteres alpha numericos na string
char* trim_it(const char* str) {
	char* res = malloc(sizeof(char) * 18);
	unsigned int i = 0, j = 0;
	int c;
	while(j < 18)
		if(isalnum(*(str+i)))
			*(res+j++) = *(str+i++);
		else
			i++;
	return res;
}

// função para comparar dois elementos settings (pela string)
int cmp_settings_strv(void *a, void *b) {
	const char* str_a = (const char*) a;
	config_setting_t* elem = config_setting_get_member((config_setting_t*)b, "cod");
	const char* str_b = config_setting_get_string(elem);
	if(strcmp(str_a, str_b) == 0)
		return 1;
	return 0;
}
