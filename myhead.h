#include <stdio.h>
void appendfile(int num, int frame_num) // append file f2 at the end of frame file
{
    char filename[20], framename[20];  
    sprintf(filename, "#%dpacket.txt", num); 
    sprintf(framename, "#%dframe.txt", frame_num);

    FILE *fr = fopen(framename, "a");
    FILE *f = fopen(filename, "r");
    char buffer[50]; // read each line in the frame as string
    if(f == NULL) 
    {
        perror("Error opening file.");
    }
    else 
    {
        while(fgets(buffer, sizeof(buffer), f)) 
        {
        fprintf(fr, "%s", buffer);
        }
    }
    fclose(f);
    fclose(fr);
}

int rows(int num)
{
    // to decide the number of lines in one frame
    char filename[20];
    sprintf(filename, "#%dframe.txt", num);
    FILE *f = fopen(filename, "r");
    char buffer[384*4];
    int count = 0;
    if(f == NULL) 
    {
        perror("Error opening file.");
    }
    else 
    {
        while(fgets(buffer, sizeof(buffer), f)) 
        {
        count = count + 1;
        }
    }
    fclose(f);
    return count;
} 