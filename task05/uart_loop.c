#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>


int main(void) {
    const char *uart = "/dev/ttyAMA0";

    int fd = open(uart, O_RDWR | O_NOCTTY);
    if (fd < 0) {
        perror("open");
        return EXIT_FAILURE;
    }

    struct termios tty;
    if (tcgetattr(fd, &tty) != 0) {
        perror("tcgetattr");
        close(fd);
        return EXIT_FAILURE;
    }

    cfmakeraw(&tty);

    cfsetispeed(&tty, B115200);
    cfsetospeed(&tty, B115200);

    tty.c_cflag |= (CLOCAL | CREAD);
    tty.c_cflag &= ~CSTOPB;
    tty.c_cflag &= ~CRTSCTS;

    tty.c_cc[VMIN]  = 0;
    tty.c_cc[VTIME] = 10;

    if (tcsetattr(fd, TCSANOW, &tty) != 0) {
        perror("tcsetattr");
        close(fd);
        return 1;
    }

    tcflush(fd, TCIOFLUSH);

    const char *msg = "Hello UART loopback!";
    size_t len = strlen(msg);

    ssize_t w = write(fd, msg, len);
    if (w < 0) {
        perror("write");
        close(fd);
        return 1;
    }
    tcdrain(fd);

    char buf[128] = {0};
    size_t got = 0;
    while (got < len) {
        ssize_t r = read(fd, buf + got, len - got);
        if (r < 0) {
            perror("read");
            break;
        }
        if (r == 0) {
            break; // timeout
        }
        got += r;
    }

    printf("Sent    (%zu): %s\n", len, msg);
    printf("Received(%zu): %s\n", got, buf);
    printf("Loopback %s\n", (got == len && memcmp(msg, buf, len) == 0) ? "PASSED" : "FAILED");

    close(fd);
    return EXIT_SUCCESS;
}
