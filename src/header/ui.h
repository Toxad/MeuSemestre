#include <libconfig.h>
#include "list.h"

#ifndef _UI_H
#define _UI_H

int lang_screen(config_t*);

int course_select_screen(config_t*);

int done_classes_screen(config_t*, Lista*, config_t*);

int result_screen(config_t*, Lista*, config_t*, unsigned int*);

#endif
