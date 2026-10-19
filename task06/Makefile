override BUILD_DIR := build
CROSS_COMPILE ?=

RPI ?= rpi

CC := $(CROSS_COMPILE)gcc
CFLAGS := -Wall -Wextra -O2 -static

.PHONY: all clean deploy install
.DEFAULT_GOAL := all

all: $(BUILD_DIR)/echo $(BUILD_DIR)/loopback

$(BUILD_DIR)/loopback: uart_loop.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o $@

$(BUILD_DIR)/echo: ECHO.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) $< -o	$@

$(BUILD_DIR):
	mkdir -p $@

deploy:
	scp $(BUILD_DIR)/loopback $(BUILD_DIR)/echo $(RPI):~

install: $(BUILD_DIR)/echo uart-echo.service
	scp $(BUILD_DIR)/echo $(RPI):/tmp/uart-echo
	scp uart-echo.service $(RPI):/tmp/uart-echo.service
	@printf '%s\n' "$$SUDO_PASS" | ssh $(RPI) "sudo -S -p '' sh -c '\
		systemctl stop uart-echo.service || true; \
		install -m 755 /tmp/uart-echo /usr/local/bin/uart-echo && \
		install -m 644 /tmp/uart-echo.service /etc/systemd/system/ && \
		systemctl daemon-reload && \
		systemctl enable --now uart-echo.service'"

clean:
	rm -rf $(BUILD_DIR)

