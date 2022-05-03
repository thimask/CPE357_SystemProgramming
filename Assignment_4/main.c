#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>
#include <signal.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <string.h>
#include <fcntl.h> 
#include <strings.h>
#include <string.h>
#include <sys/time.h> 

int fd[2];
void pipes(int i)
{
    dup2(fd[0], STDIN_FILENO);
}
 
int recursearch(struct dirent* dp, DIR*d, int parent, char*filename, char*searchword, int fs, int ws, int found, char * ftype, int fileset, char *fn, char*childreport)
{
    while ((dp = readdir(d)) != NULL)
    {
        if (strstr(".", dp->d_name)!=0 || strstr("..", dp->d_name)!=0)
        {
        }
        else if (fs)
        {
            if ((fileset == 1) && (strstr(dp->d_name, ftype) == 0))
            {
            }
            else if (strncmp((dp->d_name), filename, strlen(filename)) == 0)
            {
                found = 1;
                strcat(childreport, "Found the file: ");
                strcat(childreport, dp->d_name);
                strcat(childreport, " In the subirectory: ");
                strcat(childreport, fn);
                strcat(childreport, "\n");
            }
            else if (dp->d_type == DT_DIR)
            {
                char f[5000];
                strcpy(f, fn);
                strcat(f, "/");
                strcat(f, dp->d_name);
                DIR *d2 = opendir(f);
                found += recursearch(dp, d2, parent, filename, searchword, fs, ws, found, ftype, fileset, f, childreport);
            }
        }
        else if (ws)
        {
            struct stat st;
            stat(dp->d_name, &st);
            char f[5000];
            strcpy(f, fn);
            strcat(f, "/");
            strcat(f, dp->d_name);
            
            if (dp->d_type == DT_DIR)
            {
                DIR *d2 = opendir(f);
                found += recursearch(dp, d2, parent, filename, searchword, fs, ws, found, ftype, fileset, f, childreport);
                continue;
            }
            if ((fileset == 1) && (strstr(dp->d_name, ftype) == 0))
            {
            }
            else
            {
                FILE *file = fopen(f, "rb");
                if (file == NULL)
                {
                    fclose(file);
                    continue;
                }
                char*fbody = malloc(st.st_size);
                fread(fbody, 1, st.st_size, file);
                fclose(file);
                if (strstr(fbody, searchword) != 0)
                {
                    found = 1;
                    strcat(childreport, "Found in the file: ");
                    strcat(childreport, dp->d_name);
                    strcat(childreport, " In the subirectory path: ");
                    strcat(childreport, f);
                    strcat(childreport, "\n");
                }
            }
        }
    }
    return found;
}

int main()
{
    int parent = getpid();
    int *kpid = mmap(NULL, (10*sizeof(int)), PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    char *buffer = malloc(1073741824);
    char *inputs = mmap(NULL, 1000, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    int *numchild = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    int *kiddone = mmap(NULL, 4, PROT_READ | PROT_WRITE, MAP_ANONYMOUS | MAP_SHARED, -1, 0);
    *kiddone = 0;
    
    signal(SIGUSR1, pipes);
    int saveST = dup(STDIN_FILENO);
    pipe(fd);
    printf("\033[32m");
    printf("\nEnter \"list\" to see all child processes, their PID, and their task");
    printf("\nEnter \"kill <PID>\" to kill an individual child process and stop its find attempt");
    printf("\nEnter \"quit\" or \"q\" to end the program");
    printf("\nEnter \"find \"word\"\" to find a word in a file");
    printf("\nEnter \"find filename\" to search for a word");
    printf("\nUse the flags \"-s\" to search subdirectories or \"-f:filetype\" to search by filetype");
    fflush(0);
    

    while (1)
    {
        printf("\033[01;34m");
        printf("\nFindStuff: ");
        printf("\033[0m");
        printf("$ ");
        fflush(0);
        //restore stdin 
        dup2(saveST, STDIN_FILENO);
        int i = 0;
        while (buffer[i] != 0)
        {
            buffer[i] = 0;
            i++;
        }
        read(STDIN_FILENO, buffer, 1073741824);
        fflush(0);
        if (*kiddone)
        {
            printf("\n%s\n", buffer);
            fflush(0);
            *kiddone = 0;
        }
        
        if (strncmp("list", buffer, 4) == 0)
        {
            for (int i = 0; i < 10; i++)
            {
                if (kpid[i] == 0)
                {
                    write(STDOUT_FILENO, "\nEmpty Process\n", 16);
                    fsync(fd[1]);
                }
                else
                {
                    printf("\nPID of Process: %d Task: %s", kpid[i], inputs + i*100);
                    fflush(0);
                }   
            }
        }
        else if (strchr(buffer, 'q') != 0 || strncmp(buffer, "quit", 4) == 0)
        {
            fflush(0);
            for (int i = 0; i < 10; i++)
            {
                if (kpid[i] != 0)
                {
                    kill(kpid[i], SIGKILL);
                }
            }
            printf("\nProgram Ended.\n\n");
            munmap(&kpid, 10*sizeof(int));
            munmap(&inputs, 1000);
            munmap(&numchild, 4);
            munmap(&kiddone, 4);
            free(buffer);
            wait(0);
            return 0;
        }
        else if (strncmp("kill", buffer, 4) == 0)
        {
            int k = 0;
            char killid[10000];
            while (buffer[k] != ' ')
            {
                k++;
            }
            k++;
            int j = 0;
            while ((buffer[k]) != '\0')
            {
                killid[j] = buffer[k];
                k++;
                j++; 
            }
            killid[j] = '\0';
            int num = atoi(killid);
            for(int i = 0; i < 10; i++)
            {
                if (kpid[i] == num)
                {
                    kpid[i] = 0;
                    int h = i*100;
                    while (inputs[h] != 0 && h < (h + 100))
                    {
                        inputs[h] = 0;
                        h++;
                    }
                    (*numchild)--;
                    kill(num, SIGKILL);
                    waitpid(num, 0, 0);
                }
            }
        }
        else if (strncmp("find", buffer, 4) == 0)
        {
            int subdir = 0;
            if (*numchild == 9)
            {
                write(STDOUT_FILENO, "Limit of 10 processes reached.", 31);
                fsync(fd[1]);
            }
            else
            {
                (*numchild)++;
                if (fork() == 0)
                {
                    
                    clock_t stop, start;
                    start = clock();
                    time_t T = time(NULL);
                    struct tm tm = *localtime(&T);
                    int found = 0;
                    int z = 0;
                    for (;z < 10; z++)
                    {
                        if (kpid[z] == 0)
                        {
                            kpid[z] = getpid();
                            strcpy(inputs + z*100, buffer);
                            break;
                        }
                    }
                    if (strstr(buffer, " -s") != 0)
                    {
                        subdir = 1;
                    }
                    char ftype[5];
                    int fileset = 0;
                    if (strstr(buffer, "-f:") != 0)
                    {
                        int k = 0;
                        fileset = 1;
                        while (buffer[k] != ':')
                        {
                            k++;
                        }
                        k++;
                        int j = 1;
                        ftype[0] = '.';
                        while (buffer[k] != ' ' && buffer[k] != '\0' && buffer[k] != '\n')
                        {
                            ftype[j] = buffer[k];
                            k++; 
                            j++;
                        }
                        ftype[j] = '\0';
                        ftype[j+1] = '\0';
                    }
                    char searchword[100];
                    int ws = 0;
                    char filename[100];
                    int fs = 0;
                    if (strchr(buffer, '\"') != 0)
                    {
                        ws = 1;
                        int k = 0;
                        while (buffer[k] != '\"')
                        {
                            k++;
                        }
                        k++;
                        int j = 0;
                        while (buffer[k] != '\"')
                        {
                            searchword[j] = buffer[k];
                            k++; 
                            j++;
                        }
                        searchword[j] = '\0';
                        searchword[j+1] = '\0';
                    }
                    else if (strchr(buffer, '.') != 0)
                    {
                        fs = 1;
                        int k = 0;
                        while (buffer[k] != ' ')
                        {
                            k++;
                        }
                        k++;
                        int j = 0;
                        while (buffer[k] != '\n' && buffer[k] != ' ' && buffer[k] != '\0')
                        {
                            filename[j] = buffer[k];
                            k++; 
                            j++;
                        }
                        filename[j] = '\0';
                        filename[j+1] = '\0';
                    }
                    DIR *d = opendir(getcwd(NULL, 5000));
                    //DIR *d = opendir("/Users/Quinn/Documents/CPE357");
                    struct dirent *dp;
                    char childreport[100000];
                    strcpy(childreport, "");
                    if (subdir)
                    {
                    found = recursearch(dp, d, parent, filename, searchword, fs, ws, found, ftype, fileset, getcwd(NULL, 5000), childreport);
                    //found = recursearch(dp, d, parent, filename, searchword, fs, ws, found, ftype, fileset, "/Users/Quinn/Documents/CPE357", childreport);
                        
                    }
                    while ((dp = readdir(d)) != NULL && !subdir)
                    {
                        if (fs)
                        {
                            if (strstr(".", dp->d_name)!=0 || strstr("..", dp->d_name)!=0)
                            {
                            }
                            else if ((fileset == 1) && (strstr(dp->d_name, ftype) == 0))
                            {
                            }
                            else if (strncmp((dp->d_name), filename, strlen(filename)) == 0)
                            {
                                found = 1;
                                char *cwd;
                                cwd = getcwd(NULL, 5000);
                                strcat(childreport, "Found the file: ");
                                strcat(childreport, dp->d_name);
                                strcat(childreport, " In the Directory: ");
                                strcat(childreport, cwd);
                                strcat(childreport, "\n");
                            }
                        }
                        else if (ws)
                        {
                            struct stat st;
                            stat(dp->d_name, &st);
                            if (strstr(".", dp->d_name)!=0 || strstr("..", dp->d_name)!=0)
                            {
                                continue;
                            }
                            else if ((fileset == 1) && (strstr(dp->d_name, ftype) == 0))
                            {
                                continue;
                            }
                            else if (dp->d_type == DT_DIR)
                            {
                                continue;
                            }
                            FILE *file = fopen(dp->d_name, "rb");
                            if (file == NULL)
                            {
                                fclose(file);
                                continue;
                            }
                            char*fbody = malloc(st.st_size);
                            fread(fbody, 1, st.st_size, file);
                            fclose(file);
                            if (strstr(fbody, searchword) != 0)
                            {
                                found = 1;
                                char *cwd;
                                cwd = getcwd(NULL, 5000);
                                strcat(childreport, "Found the file: ");
                                strcat(childreport, dp->d_name);
                                strcat(childreport, " In the Directory: ");
                                strcat(childreport, cwd);
                                strcat(childreport, "\n");
                            }
                            free(fbody);
                        }
                    }
                    if (found == 0)
                    {
                        char *cwd;
                        cwd = getcwd(NULL, 5000);
                        strcpy(childreport, "Not Found");
                    }
                    fflush(0);
                    closedir(d);
                    kpid[z] = 0;
                    inputs[z*100] = 0;

                    stop = clock();
                    double timetaken = ((double)(stop - start))/CLOCKS_PER_SEC;
                    int ms = timetaken * 1000;
                    char ttime[100];
                    strcpy(ttime, "");
                    sprintf(ttime, "\nTime Duration of Completed Process: %d:%d:%d:%d", ms/(1000*60*60), (ms%(1000*60*60))/(1000*60), ((ms%(1000*60*60))%(1000*60))/1000, ((ms%(1000*60*60))%(1000*60))%1000);
                    strcat(childreport, ttime);
                    close(fd[0]);
                    write (fd[1], childreport, strlen(childreport));
                    fsync(fd[1]);
                    fsync(STDOUT_FILENO);
                    close(fd[1]);
                    *kiddone = 1;
                    kill(parent, SIGUSR1);
                    (*numchild)--;
                    return 0;
                }
            }
        }
    }
    return 0;
}
