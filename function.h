//########## Telegram ##########
//########## @Kool_2  ##########

#pragma once
#include "structs.h"
unsigned int hash(const char *str);
int add_if_unique(const char *password);
void free_hash_table();
void generate_password(int length, const char *charset, char *password);
void show_progress(int current, int total);
void *update_progress(void *args);
void *generate_password_thread(void *args);
const char *select_charset();
void generate_partial_password(char *password, int lendth, const char *charset);
void print_logo();

