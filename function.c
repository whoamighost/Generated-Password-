//########## Telegram ##########
//########## @Kool_2  ##########

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include "macros.h"
#include "structs.h"
#include "function.h"

Node *hash_table[HASH_TABLE_SIZE] = {NULL};  
pthread_mutex_t hash_mutex = PTHREAD_MUTEX_INITIALIZER;  

void print_logo() {
    const char *logo[] = {
        "                                        ,-.----.                                        ",
        "   ,----..                              \\    /  \\                                       ",
        "  /   /   \\   @Kool_2                   |   :    \\                                      ",
        " |   :     :                ,---,       |   |  .\\ :                                     ",
        " .   |  ;. /            ,-+-. /  |      .   :  |: |             .--.--.    .--.--.      ",
        " .   ; /--`     ,---.  ,--.'|'   |      |   |   \\ : ,--.--.    /  /    '  /  /    '     ",
        " ;   | ;  __   /     \\|   |  ,\"' |      |   : .   //       \\  |  :  /`./ |  :  /`./     ",
        " |   : |.' .' /    /  |   | /  | |      ;   | |`-'.--.  .-. | |  :  ;_   |  :  ;_       ",
        " .   | '_.' :.    ' / |   | |  | |      |   | ;    \\__\\/: . .  \\  \\    `. \\  \\    `.    ",
        " '   ; : \\  |'   ;   /|   | |  |/   ___ :   ' |    ,\" .--.; |   `----.   \\ `----.   \\   ",
        " '   | '/  .''   |  / |   | |--' .'  .`|:   : :   /  /  ,.  |  /  /`--'  //  /`--'  /   ",
        " |   :    /  |   :    |   |/  .'  .'   :|   | :  ;  :   .'   \\'--'.     /'--'.     /    ",
        "  \\   \\ .'    \\   \\  /'---',---, '   .' `---'.|  |  ,     .-./  `--'---'   `--'---'     ",
        "   `---`       `----'      ;   |  .'      `---`   `--`---'                              ",
        "                           `---'                                                        "
    };


    int logo_width = strlen(logo[0]);                 
    int logo_height = sizeof(logo) / sizeof(logo[0]); 

    printf(FRAME_COLOR);
    for (int i = 0; i < logo_width + 6; i++) 
    {
        printf("%c", FRAME_SYMBOL);
    }
    printf("\n");

    
    for (int i = 0; i < logo_height; i++) 
    {
        printf(FRAME_COLOR "** "); 
        printf(LOGO_COLOR "%s" RESET_COLOR, logo[i]); 
        printf(FRAME_COLOR " %c%c\n" RESET_COLOR, FRAME_SYMBOL, FRAME_SYMBOL);
    }

    printf(FRAME_COLOR);
    for (int i = 0; i < logo_width + 6; i++) 
    {
        printf("%c", FRAME_SYMBOL);
    }
    printf("\n" RESET_COLOR);
}




unsigned int hash(const char *str)  
{
    unsigned int hash = 0;  
    while (*str) 
    {
        hash = (hash << 5) + *str++;
    }
    return hash % HASH_TABLE_SIZE;
}

int add_if_unique(const char *password)
{
    unsigned int index = hash(password);
    pthread_mutex_lock(&hash_mutex);
    Node *node = hash_table[index];

    while (node)
    {
        if (strcmp(node->password, password) == 0)
        {
            pthread_mutex_unlock(&hash_mutex);
            return 0;
        }
        node = node->next;
    }
    Node *new_node = (Node *)malloc(sizeof(Node));
    new_node->password = strdup(password);
    new_node->next = hash_table[index];
    hash_table[index] = new_node;
    pthread_mutex_unlock(&hash_mutex);
    return 1;
}
void free_hash_table()
{
    for (int i = 0; i < HASH_TABLE_SIZE; i++)
    {
        Node *node = hash_table[i];
        while (node)
        {
            Node *temp = node;
            node = node->next;
            free(temp->password);
            free(temp);
        }
    }
}
void generate_password(int length, const char *charset, char *password)
{
    int charset_size = strlen(charset);
    for (int i = 0; i < length; i++)
    {
        password[i] = charset[rand() % charset_size]; 
    }
    password[length] = '\0';
}
void show_progress(int current, int total)
{
    int bar_width = 70;
    float ratio = current / (float) total;
    int pos = bar_width * ratio;

    printf("[]");
    for (int i = 0; i < bar_width; i++)
    {
        if (i < pos) printf(GREEN_COLOR"=");
        else if (i == pos) printf(GREEN_COLOR">");
        else printf(" ");
    }
    printf("] %3.0f%%\r", ratio * 100.0);
    printf(RESET_COLOR"\r");
    fflush(stdout);
}
void *update_progress(void *args)
{
    ProgressArgs *progress_args = (ProgressArgs *)args;
    int *progress = progress_args->progress;
    int num_threads = progress_args->num_threads;
    int total_password = progress_args->total_password;
    int total = 0;

    while (total < total_password)
    {
        total = 0;
        for (int i = 0; i < num_threads; i++)
        {
            total += progress[i];
        }
        show_progress(total, total_password);
        usleep(100000);
    }
    printf(GREEN_COLOR "\rProgress: %d\\%d (100%%)"RESET_COLOR"\n", total_password, total_password);
    return NULL;
}
void *generate_password_thread(void *args)
{
    ThreadArgs *thread_args = (ThreadArgs *)args;
    FILE *file = fopen(thread_args->filename, "a");
    if (!file)
    {
        perror(RED_COLOR "Error opening file! "RESET_COLOR);
        return NULL;
    }

    char password[PASSWORD_MAX_LENGTH + 1];
    int generated = 0;

    while (generated < thread_args->count)
    {
        if (strlen(thread_args->partial_password) > 0)
        {
            strcpy(password, thread_args->partial_password);
            password[strlen(thread_args->partial_password)] = '\0';

            generate_password(
                thread_args->password_length - strlen(thread_args->partial_password),
                thread_args->charset,
                password + strlen(thread_args->partial_password));
        }
        else
        {
            generate_password(thread_args->password_length, thread_args->charset, password);

        }
        if (add_if_unique(password))
        {
            fprintf(file, "%s\n", password);
            generated++;
            thread_args->progress[thread_args->thread_id]++;

        }
    }
    fclose(file);
    return NULL;
}
const char *select_charset()
{
    int choice;
    printf(BLUE_COLOR "\t\t     $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"RESET_COLOR);
    printf(BLUE_COLOR "\t\t     $                                                  $\n"RESET_COLOR);
    printf(BLUE_COLOR "\t\t     $              CHOOSE A PASSWORD TYPE              $\n"RESET_COLOR);
    printf(BLUE_COLOR "\t\t     $" RED_COLOR "\t\t    -----> TELEGRAM <-----"BLUE_COLOR"\t\t$\n"RESET_COLOR);
    printf(BLUE_COLOR "\t\t     $" RED_COLOR "\t\t    ----->  @Kool_2 <-----"BLUE_COLOR"\t\t$\n"RESET_COLOR);
    printf(BLUE_COLOR "\t\t     $                                                  $\n"RESET_COLOR);
    printf(BLUE_COLOR "\t\t     $$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$\n"RESET_COLOR);
    printf(GREEN_COLOR "==> 1." YELLOW_COLOR " Letters only (A-Z, a-z)\n"RESET_COLOR);
    printf(GREEN_COLOR "==> 2." YELLOW_COLOR " Only numbers (0-9)\n"RESET_COLOR);
    printf(GREEN_COLOR "==> 3." YELLOW_COLOR " Letters and numbers\n"RESET_COLOR);
    printf(GREEN_COLOR "==> 4." YELLOW_COLOR " Symbols\n"RESET_COLOR);
    printf(GREEN_COLOR "==> 5." YELLOW_COLOR " All characters\n"RESET_COLOR);
    scanf("%d", &choice);
    {
        switch (choice)
            {
                case 1:
                    return CHARSET_ALPHA;
                case 2:
                    return CHARSET_NUM;
                case 3:
                    return CHARSET_ALPHANUM;
                case 4:
                    return CHARSET_SPECIAL;
                case 5:
                    return CHARSET_ALL;
                default:
                    printf(GREEN_COLOR"--->"RED_COLOR "Wrong choice, default letters and numbers.\n"RESET_COLOR);
                    return CHARSET_ALL;
        
        }
    }
}
void generate_partial_password(char *password, int lendth, const char *charset)
{
    int partial_length = strlen(password);
    if (partial_length >= lendth)
    {
        printf(RED_COLOR "Password part is too long!\n"RESET_COLOR);
        return;
    }
    generate_password(lendth - partial_length, charset, password + partial_length);
}