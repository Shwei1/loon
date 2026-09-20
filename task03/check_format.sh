#!/usr/bin/env bash

print_help() {
    echo "Output clang formatter result"
    echo "Usage: ./check_format.sh [ROOT-DIR]"
}

clang_format_in_dir() {
    [ -f "$1"/.clang-format ]
}

print_format() {
    echo "==== CLANG-FORMAT VERSION 11 ===="
    if clang-format-11 --dry-run -Werror -style=file "$1"/src/*.c "$1"/include/*.h > /dev/null 2>&1; then
        echo "All filles are formatted well"
    else
        echo "Some files are formatted incorrectly"
    fi
   
    echo "================================="

    echo

    # Остання версія на моїй системі
    local clang_format_version
    clang_format_version="$(clang-format --version | cut -d ' ' -f 3)"

    echo "==== CLANG-FORMAT VERSION $clang_format_version ===="
    if clang-format --dry-run -Werror -style=file "$1"/src/*.c "$1"/include/*.h  > /dev/null 2>&1; then
        echo "All filles are formatted well"
    else
        echo "Some files are formatted incorrectly"
    fi
    echo "================================="
}


case "$#" in
    0)
        print_help
        exit 0
        ;;
    1)
        if ! clang_format_in_dir "$1"; then
            echo "Error: format file missing in $1"
            exit 1
        fi
        print_format "$1"
        ;;
    *)
        echo "Error: too many arguments"
        print_help
        exit 1
esac


