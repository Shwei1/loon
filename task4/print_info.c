#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/utsname.h>

int main(int argc, char *argv[]) {
    FILE *out = stdout;
    
    if (argc == 2) {
        char *filename = argv[1];
        out = fopen(filename, "a");
        if (out == nullptr) {
            perror("fopen");
            return EXIT_FAILURE;
        }
    } else if (argc > 2) {
        fputs("Error: too many arguments. Usage: ./print_info [OUTPUT FILENAME]\n", stderr);
    }
    
    struct utsname info = {};

    time_t now = time(nullptr);

    struct tm *local = localtime(&now);
    fprintf(out, "%s", asctime(local));

    if(uname(&info) != 0) {
        perror("uname");
        return EXIT_FAILURE;
    }

    fprintf(out, "System name: %s\n", info.sysname);
    fprintf(out, "Architecture: %s\n", info.machine);
    fprintf(out, "Kernel release: %s\n", info.release);
    fprintf(out, "Kernel build info: %s\n", info.version);
    fprintf(out, "Host name: %s\n", info.nodename);

    fclose(out);
    return EXIT_SUCCESS;
}
