# Набір вихідних файлів

- Файли взяті з офіційного git-репозиторію linux.


# Форматування з `.clang-format` офіційного репозиторію

Виконано `./make_format_11.sh dummy`, після коміту ще `./make_format_latest.sh dummy`

# Створення файлів для стилю Chromium

``` shell
clang-format-11 --style=Chromium --dump-config > chromium-11.clang-format 
clang-format --style=Chromium --dump-config > chromium-22.clang-format 
```

Далі початковий вміст `.clang-format` замінено вмістом `chromium-11.clang-format`.

Проєкт відформатовано `./make_format_11 dummy`.

`.clang-format` переписано ще раз вмістом `chromium-22.clang-format`.

Після чого відформатовано `./make_format_latest dummy`
