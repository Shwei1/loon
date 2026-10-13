override BUILD_DIR := build
CROSS_COMPILE ?= 

CC := $(CROSS_COMPILE)gcc

.PHONY: all clean deploy
.DEFAULT_GOAL := all

all: $(BUILD_DIR)/echo $(BUILD_DIR)/loopback

$(BUILD_DIR)/loopback: uart_loop.c | $(BUILD_DIR)
	$(CC) $< -o $@

$(BUILD_DIR)/echo: ECHO.c | $(BUILD_DIR)
	$(CC) $< -o	$@

$(BUILD_DIR):
	mkdir -p $@

deploy:
	scp $(BUILD_DIR)/echo $(BUILD_DIR)/echo rpi:~

clean:
	rm -rf $(BUILD_DIR)

