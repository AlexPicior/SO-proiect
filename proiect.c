#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#define BUFSIZE 256
#define STATSFILE "statistica.txt"

char *get_filename_ext(char *filename) 
{
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void stats_bmp_file(int file, int stats)
{
    char bufferOut[BUFSIZE];
    int w, h;
    read(file, &w, 2);
    read(file, &w, sizeof(int)*4);
    read(file, &w, sizeof(int));
    read(file, &h, sizeof(int));

    sprintf(bufferOut, "inaltime: %d\n", h);
    write(stats, &bufferOut, strlen(bufferOut));

    sprintf(bufferOut, "lungime: %d\n", w);
    write(stats, &bufferOut, strlen(bufferOut));
}

void stats_user_access_rights(int file, int stats, struct stat fis_stat)
{
    char bufferOut[BUFSIZE];

    if (fis_stat.st_mode & S_IRUSR) 
    {
        bufferOut[0] = 'R';
    } 
    else 
    {
        bufferOut[0] = '-';
    }

    if (fis_stat.st_mode & S_IWUSR) 
    {
        bufferOut[1] = 'W';
    } 
    else 
    {
        bufferOut[1] = '-';
    }

    if (fis_stat.st_mode & S_IXUSR) 
    {
        bufferOut[2] = 'X';
    } 
    else 
    {
        bufferOut[2] = '-';
    }

    bufferOut[3] = '\n';
    write(stats, &bufferOut, strlen(bufferOut));
}

void stats_group_access_rights(int file, int stats, struct stat fis_stat)
{
    char bufferOut[BUFSIZE];

    if (fis_stat.st_mode & S_IRGRP) 
    {
        bufferOut[0] = 'R';
    } 
    else 
    {
        bufferOut[0] = '-';
    }

    if (fis_stat.st_mode & S_IWGRP) 
    {
        bufferOut[1] = 'W';
    } 
    else 
    {
        bufferOut[1] = '-';
    }

    if (fis_stat.st_mode & S_IXGRP) 
    {
        bufferOut[2] = 'X';
    } 
    else 
    {
        bufferOut[2] = '-';
    }

    bufferOut[3] = '\n';
    write(stats, &bufferOut, strlen(bufferOut));
}

void stats_others_access_rights(int file, int stats, struct stat fis_stat)
{
    char bufferOut[BUFSIZE];

    if (fis_stat.st_mode & S_IROTH) 
    {
        bufferOut[0] = 'R';
    } 
    else 
    {
        bufferOut[0] = '-';
    }

    if (fis_stat.st_mode & S_IWOTH) 
    {
        bufferOut[1] = 'W';
    } 
    else 
    {
        bufferOut[1] = '-';
    }

    if (fis_stat.st_mode & S_IXOTH) 
    {
        bufferOut[2] = 'X';
    } 
    else 
    {
        bufferOut[2] = '-';
    }

    bufferOut[3] = '\n';
    write(stats, &bufferOut, strlen(bufferOut));
}

void write_stats(char* filename, int stats)
{
    struct stat fis_stat;
    stat(filename, &fis_stat);
    int file = open(filename, O_RDONLY);

    char bufferOut[BUFSIZE];

    if(S_ISLNK(fis_stat.st_mode))
    {
        sprintf(bufferOut, "nume legatura: %s\n", filename);
        write(stats, &bufferOut, strlen(bufferOut));
        sprintf(bufferOut, "dimensiune legatura: %ld\n", fis_stat.st_size);
        write(stats, &bufferOut, strlen(bufferOut));

        // TO DO dimensiune fisier

        sprintf(bufferOut, "drepturi de acces user legatura: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_user_access_rights(file, stats, fis_stat);
        sprintf(bufferOut, "drepturi de acces grup legatura: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_group_access_rights(file, stats, fis_stat);
        sprintf(bufferOut, "drepturi de acces altii legatura: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_others_access_rights(file, stats, fis_stat);
    }
    else
    {
        if(S_ISREG(fis_stat.st_mode))
        {
            sprintf(bufferOut, "nume fisier: %s\n", filename);
            write(stats, &bufferOut, strlen(bufferOut));
            if(!strcmp(get_filename_ext(filename), "bmp"))
            {
                stats_bmp_file(file, stats);
            }
            sprintf(bufferOut, "dimensiune: %ld\n", fis_stat.st_size);
            write(stats, &bufferOut, strlen(bufferOut));
            sprintf(bufferOut, "identificatorul utilizatorului: %d\n", fis_stat.st_uid);
            write(stats, &bufferOut, strlen(bufferOut));
            struct tm *tmInfo = localtime(&fis_stat.st_mtime);
            sprintf(bufferOut, "timpul ultimei modificari: %02d.%02d.%04d\n", tmInfo->tm_mday, tmInfo->tm_mon + 1, tmInfo->tm_year + 1900);
            write(stats, &bufferOut, strlen(bufferOut));
            sprintf(bufferOut, "contorul de legaturi: %lu\n", fis_stat.st_nlink);
            write(stats, &bufferOut, strlen(bufferOut));
            
        }
        else if(S_ISDIR(fis_stat.st_mode))
        {
            sprintf(bufferOut, "nume director: %s\n", filename);
            write(stats, &bufferOut, strlen(bufferOut));
            sprintf(bufferOut, "identificatorul utilizatorului: %d\n", fis_stat.st_uid);
            write(stats, &bufferOut, strlen(bufferOut));
        }
        sprintf(bufferOut, "drepturi de acces user: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_user_access_rights(file, stats, fis_stat);
        sprintf(bufferOut, "drepturi de acces grup: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_group_access_rights(file, stats, fis_stat);
        sprintf(bufferOut, "drepturi de acces altii: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_others_access_rights(file, stats, fis_stat);
    }

    close(file);
}

int main(int argc, char* argv[])
{
    
    int stats = open(STATSFILE, O_WRONLY);
    

    if (strcmp(get_filename_ext(argv[1]), "bmp"))
    {
        printf("Usage ./program %s\n", argv[1]);
    }

    write_stats(argv[1], stats);
    
    close(stats);

    return 0;
}