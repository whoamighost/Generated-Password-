//########## Telegram ##########
//########## @Kool_2  ##########

#pragma once
typedef struct Node
{
    char *password;
    struct Node *next;
} Node;

typedef struct
{
    int *progress;
    int num_threads;
    int total_password;
} ProgressArgs;

typedef struct
{
    int count;
    int password_length;
    const char *charset;
    const char *filename;
    int total;
    int *progress;
    int thread_id;
    char partial_password[PASSWORD_MAX_LENGTH + 1];
} ThreadArgs;

