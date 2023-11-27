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
#define SCRIPTNAME "scriptA.sh"
int no_processes = 0;

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

int write_stats(char* filepath, int stats, char* c)
{
    no_processes++;
    int file = open(filepath, O_RDWR);
    if (file == -1) {
        perror("Eroare la deschiderea fisierului");
        exit(-1);
    }
    struct stat fis_stat;
    lstat(filepath, &fis_stat);
    char *filename = get_filename(filepath);

    char bufferOut[BUFSIZE];
    int no_lines = 0;
    int pid;

    if(S_ISLNK(fis_stat.st_mode))
    {
        if ((pid = fork()) < 0)
        {
            perror("Eroare\n");
            exit(-1);
        }
        if (pid == 0)
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
            exit(1);
        }
        no_lines = 6;
        
    }
    else
    {
        stat(filepath, &fis_stat);
        if(S_ISREG(fis_stat.st_mode))
        {
            
            if(!strcmp(get_filename_ext(filename), "bmp"))
            {
                if ((pid = fork()) < 0)
                {
                    perror("Eroare\n");
                    exit(-1);
                }
                if (pid == 0)
                {
                    sprintf(bufferOut, "nume fisier: %s\n", filename);
                    write(stats, &bufferOut, strlen(bufferOut));
                    stats_bmp_file(file, stats);
                    sprintf(bufferOut, "dimensiune: %ld\n", fis_stat.st_size);
                    write(stats, &bufferOut, strlen(bufferOut));
                    sprintf(bufferOut, "identificatorul utilizatorului: %d\n", fis_stat.st_uid);
                    write(stats, &bufferOut, strlen(bufferOut));
                    struct tm *tmInfo = localtime(&fis_stat.st_mtime);
                    sprintf(bufferOut, "timpul ultimei modificari: %02d.%02d.%04d\n", tmInfo->tm_mday, tmInfo->tm_mon + 1, tmInfo->tm_year + 1900);
                    write(stats, &bufferOut, strlen(bufferOut));
                    sprintf(bufferOut, "contorul de legaturi: %lu\n", fis_stat.st_nlink);
                    write(stats, &bufferOut, strlen(bufferOut));
                    sprintf(bufferOut, "drepturi de acces user: ");
                    write(stats, &bufferOut, strlen(bufferOut));
                    stats_user_access_rights(file, stats, fis_stat);
                    sprintf(bufferOut, "drepturi de acces grup: ");
                    write(stats, &bufferOut, strlen(bufferOut));
                    stats_group_access_rights(file, stats, fis_stat);
                    sprintf(bufferOut, "drepturi de acces altii: ");
                    write(stats, &bufferOut, strlen(bufferOut));
                    stats_others_access_rights(file, stats, fis_stat);
                    exit(1);
                }
                no_lines = 10;
            }
            else
            {
                int pipe_ff[2];
                int pipe_fp[2];
                if (pipe(pipe_ff) == -1) 
                {
                    perror("Eroare la crearea pipe-ului");
                    exit(-1);
                }
                if (pipe(pipe_fp) == -1) 
                {
                    perror("Eroare la crearea pipe-ului");
                    exit(-1);
                }

                if ((pid = fork()) < 0)
                {
                    perror("Eroare\n");
                    exit(-1);
                }
                if (pid == 0)
                {
                    close(pipe_fp[0]);
                    close(pipe_fp[1]);
                    close(pipe_ff[0]);

                    int file_size = lseek(file, 0, SEEK_END);
                    lseek(file, 0, SEEK_SET);

                    sprintf(bufferOut, "nume fisier: %s\n", filename);
                    write(stats, &bufferOut, strlen(bufferOut));
                    sprintf(bufferOut, "dimensiune: %ld\n", fis_stat.st_size);
                    write(stats, &bufferOut, strlen(bufferOut));
                    sprintf(bufferOut, "identificatorul utilizatorului: %d\n", fis_stat.st_uid);
                    write(stats, &bufferOut, strlen(bufferOut));
                    struct tm *tmInfo = localtime(&fis_stat.st_mtime);
                    sprintf(bufferOut, "timpul ultimei modificari: %02d.%02d.%04d\n", tmInfo->tm_mday, tmInfo->tm_mon + 1, tmInfo->tm_year + 1900);
                    write(stats, &bufferOut, strlen(bufferOut));
                    sprintf(bufferOut, "contorul de legaturi: %lu\n", fis_stat.st_nlink);
                    write(stats, &bufferOut, strlen(bufferOut));
                    sprintf(bufferOut, "drepturi de acces user: ");
                    write(stats, &bufferOut, strlen(bufferOut));
                    stats_user_access_rights(file, stats, fis_stat);
                    sprintf(bufferOut, "drepturi de acces grup: ");
                    write(stats, &bufferOut, strlen(bufferOut));
                    stats_group_access_rights(file, stats, fis_stat);
                    sprintf(bufferOut, "drepturi de acces altii: ");
                    write(stats, &bufferOut, strlen(bufferOut));
                    stats_others_access_rights(file, stats, fis_stat);

                    char file_content[200];
                    read(file, file_content, file_size);
                    write(pipe_ff[1], file_content, file_size);

                    close(pipe_ff[1]);
                    exit(1);
                }

                no_processes++;

                if ((pid = fork()) < 0)
                {
                    perror("Eroare\n");
                    exit(-1);
                }
                if (pid == 0)
                {
                    close(pipe_fp[0]);
                    close(pipe_ff[1]);

                    dup2(pipe_ff[0], 0);
                    dup2(pipe_fp[1], 1);

                    close(pipe_fp[1]);
                    close(pipe_ff[0]);
                    
                    execlp("sh", "sh", SCRIPTNAME, c, (char *)NULL);
                    perror("Eroare la exec");
                    exit(-1);
                     
                }

                close(pipe_fp[1]);
                close(pipe_ff[0]);
                close(pipe_ff[1]);
                no_lines = 8;
                char no_correct_sentences[2];
                read(pipe_fp[0], no_correct_sentences, sizeof(char));
                no_correct_sentences[1] = 0;
                printf("Au fost identificate in total %s propozitii corecte care contincaracterul %s\n", no_correct_sentences, c);

                close(pipe_fp[0]);

            }
            
        }
        else if(S_ISDIR(fis_stat.st_mode))
        {
            if ((pid = fork()) < 0)
            {
                perror("Eroare\n");
                exit(-1);
            }
            if (pid == 0)
            {
                sprintf(bufferOut, "nume director: %s\n", filename);
                write(stats, &bufferOut, strlen(bufferOut));
                sprintf(bufferOut, "identificatorul utilizatorului: %d\n", fis_stat.st_uid);
                write(stats, &bufferOut, strlen(bufferOut));
                sprintf(bufferOut, "drepturi de acces user: ");
                write(stats, &bufferOut, strlen(bufferOut));
                stats_user_access_rights(file, stats, fis_stat);
                sprintf(bufferOut, "drepturi de acces grup: ");
                write(stats, &bufferOut, strlen(bufferOut));
                stats_group_access_rights(file, stats, fis_stat);
                sprintf(bufferOut, "drepturi de acces altii: ");
                write(stats, &bufferOut, strlen(bufferOut));
                stats_others_access_rights(file, stats, fis_stat);
                exit(1);
            }
            no_lines = 5;
        }
        
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
    if (file == -1) {
        perror("Eroare la deschiderea fisierului");
        exit(-1);
    }
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
    if (argc != 4)
    {
        perror("Numar gresit de argumente");
        exit(-1);
    }
    DIR *dir = opendir(argv[1]);
    if (dir == NULL) {
        perror("Eroare la deschiderea directorului");
        exit(-1);
    }
    int base_stats = open(STATSFILE, O_WRONLY);
    if (base_stats == -1) {
        perror("Eroare la deschiderea fisierului");
        exit(-1);
    }
    char bufferOut[BUFSIZE];
    int pid;
    int status, pidf;

    

    struct dirent *entry;

    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0) 
        {
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
            
            char file_out_path[500];
            char file_name[100];
            strcpy(file_name, entry->d_name);
            get_filename_without_ext(file_name);
            sprintf(file_out_path, "%s/%s_%s", argv[2], file_name, STATSFILE);
            int stats = open(file_out_path, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);

            int no_lines = write_stats(file_path, stats, argv[3]);
            sprintf(bufferOut, "%d\n", no_lines);
            write(base_stats, &bufferOut, strlen(bufferOut));
            close(stats);
                
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

    
    

    return 0;
}