#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <unistd.h>

static inline char toggle_case(char c)
{
	u_char uc = (u_char)c;
	if (isupper(uc)) {
		return (char)tolower(uc);
	} else if (islower(uc)) {
		return (char)toupper(uc);
	} else {
		return c;
	}
}

static void str_toggle_case(const char *restrict in, char *restrict out)
{
	size_t i = 0;
	for (; in[i] != '\0'; ++i) {
		out[i] = toggle_case(in[i]);
	}
	out[i] = '\0';
}

static ssize_t read_line(int fd, char *buf, size_t size)
{
	size_t n = 0;
	char c;

	while (true) {
		ssize_t r = read(fd, &c, 1);

		if (r < 0) {
			return -1;
		}
		if (r == 0) {
			continue;
		}

		if (c == '\n' && n > 0 && buf[n - 1] == '\r') {
			buf[n - 1] = '\0';
			return (ssize_t)(n - 1);
		}

		if (n + 1 >= size) {
			return -1; // overflow
		}
		buf[n++] = c;
	}
}

int main(int argc, char *argv[])
{
	if (argc < 2) {
		fprintf(stderr, "usage: %s <device>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *uart = argv[1];

	int fd = open(uart, O_RDWR | O_NOCTTY);

	if (fd < 0) {
		perror("open");
		return EXIT_FAILURE;
	}

	struct termios tty = { 0 };

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

	tty.c_cc[VMIN] = 1;
	tty.c_cc[VTIME] = 0;

	if (tcsetattr(fd, TCSANOW, &tty) != 0) {
		perror("tcsetattr");
		close(fd);
		return 1;
	}

	tcflush(fd, TCIOFLUSH);

	char line[1024] = { 0 };
	char outp[1024 + 2] = { 0 };

	while (true) {
		ssize_t len = read_line(fd, line, sizeof(line));
		if (len < 0) {
			perror("read_line");
			goto failure;
		}
		printf("[LOG] Got message %s\n", line);

		str_toggle_case(line, outp);
		strcat(outp, "\r\n");

		write(fd, outp, strlen(outp));
		tcdrain(fd);
	}

failure:
	close(fd);

	return EXIT_SUCCESS;
}
