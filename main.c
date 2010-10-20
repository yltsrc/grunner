#include <dirent.h>
#include <glib.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pwd.h>

#define CMDLEN NAME_MAX+1
#define DIRECTORY "/usr/share/applications/"
#define NAME "Name="
#define HIDDEN "NoDisplay=true"
#define EXEC "Exec="

void concat_applications(DIR *mydir, char *dir, FILE *finput, int fdin, int fdout) {

    char *fn = (char *)malloc(CMDLEN+strlen(dir));
    char *buffer = (char *)malloc(CMDLEN);
    char *exec = (char *)malloc(CMDLEN);
    gboolean hidden;
    FILE *fp;
    struct dirent *entry = NULL;

    while((entry = readdir(mydir)))
    {
        hidden = FALSE;

        strcpy(fn, dir);
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
}

int main(void)
{
    char *cmd = (char *)malloc(CMDLEN);
    int fdout;
    int fdin;
    GPid child_pid;
    FILE *finput;
    FILE *foutput;
    char *argv[] = {"/bin/sh", "-c", "gpicker --name-separator=\"\\n\" -", 0};
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

        DIR *mydir = opendir(DIRECTORY);
        concat_applications(mydir, DIRECTORY, finput, fdin, fdout);
        closedir(mydir);

        uid_t id = getuid();
        struct passwd *pw = getpwuid(id);
        char *hd = (char *)malloc(CMDLEN);
        strcpy(hd, pw->pw_dir);
        strcat(hd, "/.local/share/applications/");

        DIR *homedir = opendir(hd);
        concat_applications(homedir, hd, finput, fdin, fdout);
        closedir(homedir);

        fclose(finput);

        fgets(cmd, CMDLEN, foutput);

        fclose(foutput);
    }

    if (strlen(cmd) > 0) {
        popen(cmd, "r");
    }

    return EXIT_SUCCESS;
}

