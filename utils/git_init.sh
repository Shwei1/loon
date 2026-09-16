#!/usr/bin/env bash

if [ ! -d ".git" ]; then
    echo "Current directory already is a git repository. Aborting." >&2
    exit 1
fi

case "$#" in
    1)
        echo "Called with no args"
        ;;
    2)
        echo "Called with 1 arg"
        ;;
    3)
        echo "Called with 2 args"
        ;;
    *)
        echo "Too many args"
        ;;
esac

