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



