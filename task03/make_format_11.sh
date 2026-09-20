#!/usr/bin/env bash

print_help() {
    echo "Script to format .c and .h files in a given project."
    echo "Usage: ./make_format_11.sh [PROJECT ROOT DIRECTORY]"
}

format_dir() {
    if ! [ -f "$1"/.clang-format ]; then
        echo "Error: no .clang-format file in the given directory." >&2
        exit 1
    fi
    clang-format-11 -i "$1"/src/*.c "$1"/include/*.h
}

case "$#" in
    0)
        print_help
        exit 0
        ;;
    1)
        format_dir "$1"
        exit 0
        ;;
    *)
        echo "Error: too many arguments." >&2
        print_help
        exit 1
        ;;
esac
