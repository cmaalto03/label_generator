#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LINE_SIZE 128
#define ITEMS_PER_LINE 3

enum selections {
    QUIT = 0,
    VIEW = 1,
    PRINT = 2,
    ADD = 3
};

typedef enum selections Selections;

char* add_prompts[] = {"Enter full name: ", "Enter address: ", "Enter town: ", "Enter abbreviated state: ", "Enter zip: "};

void get_choice(int* user_choice);
void clear_keyboard_buffer(void);

void view_labels(FILE* fpIn);
void copy_and_format(FILE* fpIn, FILE* fpOut);
void print_labels();
void add_labels(FILE* fpIn, char* filename);

int main(int argc, char* argv[])
{
    int user_choice;

    char* filenameInput = "labels_input.txt";
    
    char* filenameOutput = "labels_output.txt";

    FILE* fpIn = fopen(filenameInput, "r");
    FILE* fpOut = fopen(filenameOutput, "w");


    if (fpIn == NULL)
    {
        printf("Unable to open %s -- exiting", filenameInput);
    }

    if (fpOut == NULL)
    {
        printf("Unable to open %s -- exiting", filenameOutput);
    }

    while (user_choice)
    {
        get_choice(&user_choice);

        switch (user_choice)
        {
            case VIEW:
                view_labels(fpIn);
                break;
            case PRINT:
                view_labels(fpOut);
                copy_and_format(fpIn, fpOut);
                print_labels();
                break;
            case ADD:
                add_labels(fpIn, filenameInput);
                break;
        }
    }

    return 0;
}

void get_choice(int* user_choice)
{
    int noc;
    printf("\n[0] to Quit\n[1] to View\n[2] to Print\n[3] to Add\n[4] to Remove\n\nSelection: ");
    noc = scanf("%d", user_choice);
    printf("\n");
    clear_keyboard_buffer();
  
    if (*user_choice < -1 || *user_choice > 5 || noc != 1)
    {
        printf("I'm sorry, you entered an invalid input\n");
        get_choice(user_choice);
    }
}

void clear_keyboard_buffer()
{
    char c = 'c';

    while (c != '\n')
    {
        scanf("%c", &c);
    }
}

void view_labels(FILE* fpIn)
{
    fseek(fpIn, 0, SEEK_SET);
    int noc;
    char charBuffer;

    noc = fscanf(fpIn, "%c", &charBuffer);

    while (noc != EOF)
    {
        printf("%c", charBuffer);
        noc = fscanf(fpIn, "%c", &charBuffer);
    }

    noc = 0;
}

void add_labels(FILE* fpIn, char* filename)
{
    fpIn = fopen(filename, "a");

    if (fpIn == NULL)
    {
        printf("Unable to open %s -- exiting", filename);
    }

    int noc;
    char input;

    for (int i = 0; i < 5; i++)
    {
        printf("%s", add_prompts[i]);
        noc = scanf("%c", &input);
        while (noc)
        {
            if (input == '\n')
            {
                if (i == 1 || i == 0)
                {
                    fprintf(fpIn, "%c", '\n');
                }
                if (i == 2)
                {
                    fprintf(fpIn, "%s", ", ");
                }
                if (i == 3)
                {
                    fprintf(fpIn, "%c", ' ');
                }
                if (i == 4)
                {
                    fprintf(fpIn, "%s", "\n\n");
                }
                break;
            }
            fprintf(fpIn, "%c", input);
    
            noc = scanf("%c", &input);
        }   
    }

    fclose(fpIn);
}   

void print_labels()
{
    char line[LINE_SIZE];
    int current_line;
    FILE *pipe;
    
    pipe = popen("./print_script.sh", "r");
    if (pipe == NULL)
    {
        printf("Unable to execute -- exiting\n");
        exit(1);    
    }

    current_line = 1;
    while (fgets(line, LINE_SIZE, pipe) != NULL) 
    {
        printf("Script Output: %d: %s", current_line, line);
        ++current_line;
    }

    pclose(pipe); 
}

void copy_and_format(FILE* fpIn, FILE* fpOut)
{
    int i = 0;              
    int lineCount = 1;     
    char line[LINE_SIZE];         
    char* buffer[1024][3];  

    fseek(fpIn, 0, SEEK_SET);

    while (fgets(line, sizeof(line), fpIn) != NULL)
    {
       
        line[strcspn(line, "\n")] = '\0';

        if (lineCount % 4 == 1) {
            buffer[i][0] = malloc(strlen(line) + 1);  
            if (buffer[i][0] == NULL) 
            {
                printf("Memory allocation failed -- exiting\n");
                exit(1);  
            }
            strcpy(buffer[i][0], line);
        }

        if (lineCount % 4 == 2) {
            buffer[i][1] = malloc(strlen(line) + 1); 
            if (buffer[i][1] == NULL) 
            {
                printf("Memory allocation failed -- exiting\n");
                exit(1); 
            }
            strcpy(buffer[i][1], line);
        }

        if (lineCount % 4 == 3) {
            buffer[i][2] = malloc(strlen(line) + 1); 
            if (buffer[i][2] == NULL) 
            {
                printf("Memory allocation failed -- exiting\n");
                exit(1);  
            }
            strcpy(buffer[i][2], line);
            i++;  
        }
        lineCount++;  
    }

    int rows = i;

    for (int x = 0; x < rows; x += 3) 
    {
        fprintf(fpOut, "%-55s%-55s%-55s\n", buffer[x][0], (x+1 < rows ? buffer[x+1][0] : ""), (x+2 < rows ? buffer[x+2][0] : ""));

        fprintf(fpOut, "%-55s%-55s%-55s\n", buffer[x][1], (x+1 < rows ? buffer[x+1][1] : ""), (x+2 < rows ? buffer[x+2][1] : ""));

        fprintf(fpOut, "%-55s%-55s%-55s\n\n", buffer[x][2], (x+1 < rows ? buffer[x+1][2] : ""), (x+2 < rows ? buffer[x+2][2] : ""));
    }

    fclose(fpIn);
    fclose(fpOut);
}
