#include <ctype.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>

static volatile sig_atomic_t should_terminate = 0;

static void handle(int signal)
{
	(void)signal;
	should_terminate = 1;
}

static void timestamp(char *buf, size_t size)
{
	time_t now = time(NULL);
	struct tm tim;
	localtime_r(&now, &tim);
	if (strftime(buf, size, "%y%m%d_%H%M%S", &tim) == 0) {
		fprintf(stderr, "strftime failed\n");
		return;
	};
}

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
			if (errno == EINTR && !should_terminate) {
				continue;
			}
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
	struct sigaction sa = { 0 };
	sa.sa_handler = &handle;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;

	sigaction(SIGINT, &sa, NULL);
	sigaction(SIGTERM, &sa, NULL);

	if (argc < 2) {
		fprintf(stderr, "usage: %s <device>\n", argv[0]);
		return EXIT_FAILURE;
	}

	char logfile_name[256] = { 0 };
	char timestamp_str[64] = { 0 };
	timestamp(timestamp_str, sizeof(timestamp_str));
	strncat(logfile_name, "log_", 128);
	strncat(logfile_name, timestamp_str, 128);
	strncat(logfile_name, ".log", 128);

	FILE *log = fopen(logfile_name, "a");
	if (!log) {
		perror("fopen");
		return EXIT_FAILURE;
	}

	const char *uart = argv[1];

	int fd = open(uart, O_RDWR | O_NOCTTY);

	if (fd < 0) {
		timestamp(timestamp_str, sizeof(timestamp_str));
		fprintf(log, "[%s] UART initialization failed\n",
			timestamp_str);
		fclose(log);
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

	while (!should_terminate) {
		ssize_t len = read_line(fd, line, sizeof(line));
		if (len < 0) {
			if (!should_terminate) {
				perror("read_line");
			}
			break;
		}
		timestamp(timestamp_str, sizeof(timestamp_str));
		fprintf(log, "[%s] Got message %s\n", timestamp_str, line);
		fflush(log);

		str_toggle_case(line, outp);
		strcat(outp, "\r\n");

		write(fd, outp, strlen(outp));
		tcdrain(fd);
	}

	close(fd);
	fclose(log);

	return EXIT_SUCCESS;
}
