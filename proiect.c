#include <stdio.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <time.h>
#include <dirent.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <errno.h>
#define BUFSIZE 256
#define STATSFILE "statistica.txt"

char *get_filename_ext(char *filename) 
{
    char *dot = strrchr(filename, '.');
    if(!dot || dot == filename) return "";
    return dot + 1;
}

void get_filename_without_ext(char *filename) 
{
    for (int i = 0; i < strlen(filename); i++)
    {
        if (filename[i] == '.')
        {
            filename[i] = 0;
        }
        
    }
    
}

char *get_filename(char *filepath) 
{
    char *dot = strrchr(filepath, '/');
    if(!dot || dot == filepath) return "";
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

int write_stats(char* filepath, int stats)
{
    struct stat fis_stat;
    lstat(filepath, &fis_stat);
    int file = open(filepath, O_RDWR);
    char *filename = get_filename(filepath);

    char bufferOut[BUFSIZE];
    int no_lines = 0;

    if(S_ISLNK(fis_stat.st_mode))
    {
        struct stat target_stat;
        stat(filepath, &target_stat);
        sprintf(bufferOut, "nume legatura: %s\n", filename);
        write(stats, &bufferOut, strlen(bufferOut));
        sprintf(bufferOut, "dimensiune legatura: %ld\n", fis_stat.st_size);
        write(stats, &bufferOut, strlen(bufferOut));
        sprintf(bufferOut, "dimensiune fisier: %ld\n", target_stat.st_size);
        write(stats, &bufferOut, strlen(bufferOut));
        sprintf(bufferOut, "drepturi de acces user legatura: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_user_access_rights(file, stats, fis_stat);
        sprintf(bufferOut, "drepturi de acces grup legatura: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_group_access_rights(file, stats, fis_stat);
        sprintf(bufferOut, "drepturi de acces altii legatura: ");
        write(stats, &bufferOut, strlen(bufferOut));
        stats_others_access_rights(file, stats, fis_stat);
        no_lines = 6;
    }
    else
    {
        stat(filepath, &fis_stat);
        if(S_ISREG(fis_stat.st_mode))
        {
            no_lines = 8;
            sprintf(bufferOut, "nume fisier: %s\n", filename);
            write(stats, &bufferOut, strlen(bufferOut));
            if(!strcmp(get_filename_ext(filename), "bmp"))
            {
                stats_bmp_file(file, stats);
                no_lines = 10;
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
            no_lines = 5;
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

    return no_lines;
}

typedef struct {
    unsigned char blue;
    unsigned char green;
    unsigned char red;
} BMPixel;

void convert_img(char* filepath)
{
    int file = open(filepath, O_RDWR);
    char header[54];
    read(file, header, sizeof(char) * 54);

    int width = *(int*)&header[18];
    int height = *(int*)&header[22];

    BMPixel pixel;
    lseek(file, 54, SEEK_SET);  

    for (int i = 0; i < width * height; ++i) {
        read(file, &pixel, sizeof(BMPixel));

        pixel.red = (unsigned char)(0.299 * pixel.red);
        pixel.green = (unsigned char)(0.587 * pixel.green);
        pixel.blue = (unsigned char)(0.114 * pixel.blue);

        lseek(file, -sizeof(BMPixel), SEEK_CUR);
        write(file, &pixel, sizeof(BMPixel));
    }
    
    close(file);
}


int main(int argc, char* argv[])
{
    
    DIR *dir = opendir(argv[1]);
    int base_stats = open(STATSFILE, O_WRONLY);
    char bufferOut[BUFSIZE];
    int pid;
    int no_processes = 0;
    int status, pidf;

    if (dir == NULL) {
        perror("Eroare la deschiderea directorului");
        exit(-1);
    }

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
        {
            no_processes++;
            char file_path[500];
            sprintf(file_path, "%s/%s", argv[1], entry->d_name);

            if (!strcmp(get_filename_ext(entry->d_name), "bmp"))
            {
                no_processes++;
                if ((pid = fork()) < 0)
                {
                    perror("Eroare\n");
                    exit(-1);
                }
                if (pid == 0)
                {
                    convert_img(file_path);
                    exit(1);
                }
            }

            if ((pid = fork()) < 0)
            {
                perror("Eroare\n");
                exit(-1);
            }
            if (pid == 0)
            {
                char file_out_path[500];
                char file_name[100];
                strcpy(file_name, entry->d_name);
                get_filename_without_ext(file_name);
                sprintf(file_out_path, "%s/%s_%s", argv[2], file_name, STATSFILE);
                int stats = open(file_out_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

                int no_lines = write_stats(file_path, stats);
                sprintf(bufferOut, "%d\n", no_lines);
                write(base_stats, &bufferOut, strlen(bufferOut));
                close(stats);
                exit(1);
            }
        }
    }

    for (int i = 0; i < no_processes; i++)
    {
        if ((pidf = wait(&status)) < 0)
        {
            perror("eroare\n");
            exit(-1);
        }
        if (WIFEXITED(status))
        {
            printf("S-a încheiat procesul cu pid-ul %d si codul %d\n", pidf, WEXITSTATUS(status));
        }
    }
    
    
    close(base_stats);
    closedir(dir);

    // int file = open(argv[1], O_RDWR);
    // if (file == -1) {
    //     perror("Eroare la deschiderea fisierului");
    //     exit(-1);
    // }

    // convert_img(file);

    // close(file);
    

    return 0;
}