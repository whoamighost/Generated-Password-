//########## Telegram ##########
//########## @Kool_2  ##########

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include "macros.h"
#include "structs.h"
#include "function.h"

int main()
{
    srand(time(NULL));
    print_logo();

    int total_password, password_length, num_threads;
    char partial_password[PASSWORD_MAX_LENGTH + 1] = {0};
    char password[PASSWORD_MAX_LENGTH + 1] = {0};
    int pass;

    const char *charset = select_charset();
    
    printf(GREEN_COLOR "----> "BLUE_COLOR "Enter the number of passwords! "GREEN_COLOR "---->  "RESET_COLOR);
    scanf("%d", &total_password);
    printf(GREEN_COLOR "----> " BLUE_COLOR "Enter the password length (up to %d) "GREEN_COLOR "---->  "RESET_COLOR, PASSWORD_MAX_LENGTH);
    scanf("%d", &password_length);

    if (password_length > PASSWORD_MAX_LENGTH) password_length = PASSWORD_MAX_LENGTH;

    printf(GREEN_COLOR "----> "BLUE_COLOR "Enter the number of threads! "GREEN_COLOR "---->  " RESET_COLOR);
    scanf("%d", &num_threads);

    printf(GREEN_COLOR "----> "BLUE_COLOR "Enter part of the password (if there is 1: no 0) "GREEN_COLOR "---->  " RESET_COLOR);
    scanf("%d", &pass);
    
    if (pass == 1)
    {
        printf(GREEN_COLOR"----> "BLUE_COLOR "Enter part of the password! "GREEN_COLOR "---->  "RESET_COLOR);
        scanf("%s", partial_password);

        if (strlen(partial_password) > password_length)
        {
            printf(GREEN_COLOR "----> "RED_COLOR "Password part is too long!\n"RESET_COLOR);
            return 1;
        }
        strcpy(password, partial_password);
        generate_partial_password(password, password_length, charset);
    }
    else 
    {
        generate_password(password_length, charset, password);
    }
    

    int progress[num_threads];
    memset(progress, 0, sizeof(progress));

    pthread_t threads[num_threads];
    pthread_t progress_thread;
    ThreadArgs thread_args[num_threads];
    ProgressArgs progress_args;

    progress_args.progress = progress;
    progress_args.num_threads = num_threads;
    progress_args.total_password = total_password;

    int password_per_thread = total_password / num_threads;
    int remaining_password = total_password % num_threads;

    for (int i = 0; i < num_threads; i++)
    {
        thread_args[i].count = password_per_thread + (i < remaining_password ? 1 :0);
        thread_args[i].password_length = password_length;
        thread_args[i].charset = charset;
        thread_args[i].filename = "passwords.txt";
        thread_args[i].total = total_password;
        thread_args[i].progress = progress;
        thread_args[i].thread_id = i;
        strcpy(thread_args[i].partial_password, partial_password);

        pthread_create(&threads[i], NULL, generate_password_thread, &thread_args[i]);
    }

    pthread_create(&progress_thread, NULL, update_progress, &progress_args);

    for (int i = 0; i < num_threads; i++)
    {
        pthread_join(threads[i], NULL);
    }
    pthread_join(progress_thread, NULL);

    free_hash_table();
    printf("\n");
    printf(RED_COLOR "-------> Telegram <-------\n"RESET_COLOR);
    printf(RED_COLOR "------->  @Kool_2 <-------\n"RESET_COLOR);
    printf("\n");
    printf(GREEN_COLOR "\n$$$$$--> Generation completed! <--$$$$$\n"RESET_COLOR);

    return 0;
}