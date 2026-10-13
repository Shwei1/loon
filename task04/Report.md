# Завдання №4

## Компіляція програми на host

``` shell
./build.sh print_info.c host
```

Результат:
```
Fri Sep 25 02:13:07 2026
System name: Linux
Architecture: x86_64
Kernel release: 7.2.6-arch2-1
Kernel build info: #1 SMP PREEMPT_DYNAMIC Mon, 14 Sep 2026 22:41:30 +0000
Host name: sealarch
```

Через `readelf -h print_info` побачимо основну інформацію, зокрема архітектуру:
```
Machine:                           Advanced Micro Devices X86-64
```

Динамічні залежності через `ldd`:

```
  	linux-vdso.so.1 (0x00007fb81a4ff000)
	libc.so.6 => /usr/lib/libc.so.6 (0x00007fb81a200000)
	/lib64/ld-linux-x86-64.so.2 => /usr/lib64/ld-linux-x86-64.so.2 (0x00007fb81a501000)
```

Бачимо GNU libc, віртуальну динамічну бібліотеку лінукса і віртуальну динамічну бібліотеку самого динамічного лінкера.

Розміри секцій через `size`:

```
   text	   data	    bss	    dec	    hex	filename
   3091	    672	     48	   3811	    ee3	print_info
```

Через `strings` можна подивитися символи, які збереглися в elf файлі. Там є ті самі позначки секцій програми, символи компілятора, назви функцій тощо.

# Крос-компіляція для Raspberry Pi

``` shell
./build.sh print_info.c rpi
```

Скопіюємо виконувальний файл через SSH:
``` shell
scp print_info pi@raspberrypi.local:~
```

Результат виконання на таргеті:
```
Fri Sep 25 12:42:18 2026
System name: Linux
Architecture: aarch64
Kernel release: 6.18.50+rpt-rpi-2712
Kernel build info: #1 SMP PREEMPT Debian 1:6.18.50-1+rpt1 (2026-09-11)
Host name: raspberrypi
```

Перевіримо архітектуру через `readelf -h`:

```
Machine: AArch64
```

Розмір секцій через `size`:
```
   text    data     bss     dec     hex filename
   3041     720       8    3769     eb9 print_info
```


## Компіляція на Raspberry Pi

``` shell
./build.sh print_info.c host # Тепер host це raspberry
```

Одразу хочеться перевірити `strings`, і можна побачити, що крос-компільований бінарний файл має новіші символи GCC, адже на raspberry стоїть старий toolchain:
```
GCC: (Debian 14.2.0-19) 14.2.0
```
Тоді як на файлі, пересланому з хоста, символ був
```
GCC: (GNU) 16.1.0
```

При цьому обидва файли виконуються без проблем, отже ABI змінилося не критично.

Є незначна різниця в розмірі:
```
   text    data     bss     dec     hex filename
   3053     720       8    3781     ec5 print_info
```

## Статична компіляція на Raspberry Pi

``` shell
./build.sh print_info.c host -static
```

`ldd` скаже, що це не динамічний виконувальний файл:
```
not a dynamic executable
```

Перевіримо розмір через `size`:
```
   text    data     bss     dec     hex filename
 649696   24340   22240  696276   a9fd4 print_info
```

Бінарний файл у 184 рази більший за обсягом, його розмір майже 1 МБ.

# Статична компіляція на хості

Розмір виконувального файлу через `size`:
```
   text    data     bss     dec     hex filename
 909609   24512   23264  957385   e9bc9 print_info
```

Розмір більший в 250 разів.

Перевіримо також кількість символів:
``` shell
strings print_info | wc -l
```

Отримаємо аж 5574 символи. Все, чим розпоряджався `.so`-об'єкт з динамічним виконувальним файлом, тепер знаходиться всередині статичної програми.

## Крос-компіляція з хоста на Raspberry Pi

``` shell
./build.sh print_info.c rpi -static
```

``` shell
scp print_info pi@raspberrypi.local:~
```

Розмір трохи більший:
```
   text    data     bss     dec     hex filename
 624804   23004   22336  670144   a39c0 print_info
```

Кількість символів у `strings`: 5579. 


## Висновки
Цього разу мені пощастило і проблем з крос-компіляцією не виникло. Навіть враховуючи різницю у версіях компіляторів на платформах, результати вийшли схожими і програма запрацювала справно.


