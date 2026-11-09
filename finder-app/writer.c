#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <syslog.h>
#include <errno.h>
#include <libgen.h>

#define MAX_PATH_LEN 1024

// Helper function to simulate "mkdir -p"
int mkdir_p(const char *path) {
    char tmp[MAX_PATH_LEN];
    char *p = NULL;
    size_t len;

    snprintf(tmp, sizeof(tmp), "%s", path);
    len = strlen(tmp);
    if (len == 0) {
        return 0;
    }
    
    // Remove trailing slash if any
    if (tmp[len - 1] == '/') {
        tmp[len - 1] = 0;
    }

    for (p = tmp + 1; *p; p++) {
        if (*p == '/') {
            *p = 0;
            // Create sub-directory if it doesn't exist
            if (mkdir(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
                if (errno != EEXIST) {
                    return -1;
                }
            }
            *p = '/';
        }
    }
    
    if (mkdir(tmp, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH) != 0) {
        if (errno != EEXIST) {
            return -1;
        }
    }
    return 0;
}

int main(int argc, char *argv[]) {
    // 1. Initialize syslog with LOG_USER facility
    openlog("write_utility", LOG_PID | LOG_CONS, LOG_USER);

    // 2. Validate argument count
    if (argc != 3) {
        fprintf(stderr, "Not enough param\n");
        syslog(LOG_ERR, "Invalid number of arguments provided: expected 2, got %d", argc - 1);
        closelog();
        return 1;
    }

    char *writefile = argv[1];
    char *writestr = argv[2];

    // 3. Extract parent directory path to replicate: mkdir -p "$(dirname "$writefile")"
    char path_buffer[MAX_PATH_LEN];
    char dir_buffer[MAX_PATH_LEN];
    snprintf(path_buffer, sizeof(path_buffer), "%s", writefile);
    
    char *dir_name = dirname(path_buffer);
    snprintf(dir_buffer, sizeof(dir_buffer), "%s", dir_name);

    // Create the directory path if it does not exist
    if (mkdir_p(dir_buffer) != 0) {
        fprintf(stderr, "Failed to create directory structure\n");
        syslog(LOG_ERR, "mkdir -p failed for directory '%s': %s", dir_buffer, strerror(errno));
        closelog();
        return 1;
    }

    // 4. Open file in append mode (Simulates both touch and >> redirection)
    FILE *fp = fopen(writefile, "a");
    if (fp == NULL) {
        fprintf(stderr, "Failed to open or create file\n");
        syslog(LOG_ERR, "Failed to open target file '%s': %s", writefile, strerror(errno));
        closelog();
        return 1;
    }

    // 5. Write the string and append a newline (matching bash 'echo' behavior)
    if (fprintf(fp, "%s\n", writestr) < 0) {
        fprintf(stderr, "Failed to write data to file\n");
        syslog(LOG_ERR, "Grep/Write stream error while executing target payload to '%s'", writefile);
        fclose(fp);
        closelog();
        return 1;
    }
    fclose(fp);

    // 6. Trigger mandatory LOG_DEBUG tracking event
    syslog(LOG_DEBUG, "Writing %s to %s", writestr, writefile);

    // Release system resources and exit
    closelog();
    return 0;
}
