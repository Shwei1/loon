# Завдання №5

## Перевірка справності інтерфейсу UART

Спочатку під'єднаємо дріт між пінами 8 (TXD) та 10 (RXD). Далі треба включити цей інтерфейс на самій платі через `raspi-config` (далі в tui: Interface Options -> Serial Port -> enable serial port hardware). 
Запускаємо:
``` shell
$ build/loopback
```

```
Sent (20): Hello UART loopback!
Received(20): Hello UART loopback!
Loopback PASSED
```

## Під'єднання до HOST системи
Додамо таргет для перекидання на TARGET систему
``` Makefile
 deploy:
	scp $(BUILD_DIR)/echo $(BUILD_DIR)/echo rpi:~
```

Скомпілюємо все для TARGET:
``` shell
$ make CROSS_COMPILE=aarch64-linux-gnu- all && make deploy
```
```
mkdir -p build
aarch64-linux-gnu-gcc ECHO.c -o build/echo
aarch64-linux-gnu-gcc uart_loop.c -o build/loopback
scp build/loopback build/echo rpi:~
loopback                                                                                                                     100%   70KB   2.2MB/s   00:00
echo                                                                                                                         100%   70KB   2.8MB/s   00:00
```

Аби протестувати роботу програми, треба налаштувати серійний термінал. Я використовую адаптер USB-UART, що індексується як `/dev/ttyUSB0`:
``` shell
$ stty -F /dev/ttyUSB0 -echo
```

Тепер на TARGET:
``` shell
$ ./echo "/dev/ttyAMA0"
```
 На хості почнемо писати в дескриптор адапетра і читати з нього на фоні.
``` shell
$ cat /dev/ttyUSB0 &
$ printf 'hello\r\n' > /dev/ttyUSB0
```
Отримуємо у відповідь `HELLO`.
``` shell
$ printf 'mY nAmE iS wHaT?\r\n' > /dev/ttyUSB0
```
```
My NaMe Is WhAt?
```

Програма працює справно.


## Автостарт через systemD

Треба написати unit file. Краще також перенести бінарник кудись в `/usr/local/bin/`.
Також створимо окрему директорію для логів і скажемо systemD встановлювати робочу директорію процесу саме там (`LogsDirectory=uart-echo`, `WorkingDirectory=%L/uart-echo`).
Запустимо сервіс і перезавантажимо raspberry.

```
● uart-echo.service - UART case-toggle echo
     Loaded: loaded (/etc/systemd/system/uart-echo.service; enabled; preset: enabled)
     Active: active (running) since Wed 2026-10-14 12:37:52 BST; 4min 34s ago
 Invocation: 0a18a68fa1a340578cbec3145595424b
   Main PID: 993 (uart-echo)
      Tasks: 1 (limit: 1140)
        CPU: 8ms
     CGroup: /system.slice/uart-echo.service
             └─993 /usr/local/bin/uart-echo /dev/ttyAMA0
```

Сервіс запустився і працює справно
