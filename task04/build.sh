#!/usr/bin/env bash

print_help() {
    echo "Usage: ./build.sh [FILE] [PLATFORM] [OPTIONS]"
}

set_comp_params() {
    FLAGS="$2"
    if [[ ! -z "$FLAGS" && ! "$FLAGS" == "-static" ]]; then
        echo "Error: unknown flags $FLAGS. Must be empty or '-static'"
        return 1
    fi
    case "$1" in
        "host")
            CC="gcc"
            ;;
        "rpi")
            CC="aarch64-linux-gnu-gcc"
            ;;
        *)
            echo "Error: unknown target." 1>&2
            return 1
    esac
    
}

compile() {
    local infile="$1"
    local outname="${infile%.*}"
    "$CC" "$infile" "$FLAGS" -std=c23 -o "$outname"
}


case "$#" in
    0)
        print_help
        exit 0
        ;;
    1)
        set_comp_params "host"
        compile "$1" 
        ;;
    2)
        if ! set_comp_params "$2"; then
            exit 1
        fi
        if ! compile "$1"; then
            exit 1
        fi
        ;;
    3)
        if ! set_comp_params "$2" "$3"; then
            exit 1
        fi
        if ! compile "$1"; then
            exit 1
        fi
        ;;
    *)
        echo "Error: too many arguments."
        print_help 1>&2
        exit 1
        ;;
    esac

