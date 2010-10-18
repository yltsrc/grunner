#include <dirent.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CMDLEN 256
#define DIRECTORY "/usr/share/applications/"
#define NAME "Name="
#define HIDDEN "NoDisplay=true"
#define EXEC "Exec="

int main(void)
{
    char *fn = (char *)malloc(CMDLEN+strlen(DIRECTORY));
    char *buffer = (char *)malloc(CMDLEN);
    char *exec = (char *)malloc(CMDLEN);
    char *cmd = (char *)malloc(CMDLEN);
    gboolean hidden;
    DIR *mydir = opendir(DIRECTORY);
    FILE *fp;
    FILE *finput;
    FILE *foutput;
    int fdout;
    int fdin;
    GPid child_pid;
    char *argv[] = {"/bin/sh", "-c", "gpicker --name-separator=\"\\n\" -", 0};
    struct dirent *entry = NULL;

    gboolean ok = g_spawn_async_with_pipes(0, // work dir
                                           argv,
                                           0, //envp
                                           0, // flags
                                           0, 0, //child setup & user data
                                           &child_pid,
                                           &fdin, // stdin
                                           &fdout, //stdout
                                           0, //stderr
                                           0);

    if (ok) {

        if ((finput = fdopen(fdin, "w")) == NULL) {
            return 1;
        }
        if ((foutput = fdopen(fdout, "r")) == NULL) {
            return 2;
        }

        while((entry = readdir(mydir)))
        {
            hidden = FALSE;

            strcpy(fn, DIRECTORY);
            strcat(fn, entry->d_name);

            if((fp = fopen(fn, "r")) != NULL) {
                while( fgets(buffer, CMDLEN, fp) != NULL ) {
                    if (!strncmp(buffer, HIDDEN, strlen(HIDDEN))) {
                        hidden = TRUE;
                    }

                    if (!strncmp(buffer, EXEC, strlen(EXEC))) {
                        strcpy(exec, buffer+strlen(EXEC));
                    }
                }
            }

            if (!hidden) {
                fprintf(finput, "%s", exec);
            }

            fclose(fp);

        }

        closedir(mydir);

        fclose(finput);

        fgets(cmd, CMDLEN, foutput);

        fclose(foutput);
    }

    if (strlen(cmd) > 0) {
        popen(cmd, "r");
    }

    return EXIT_SUCCESS;
}

