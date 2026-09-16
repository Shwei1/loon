#!/usr/bin/env bash

take_input() {
    read -pr "username: " USER_NAME
    read -pr "email: " USER_EMAIL
    read -pr "default branch name: " USER_BRANCH

    export USER_NAME USER_EMAIL USER_BRANCH
}


if [ -d ".git" ]; then
    echo "Current directory already is a git repository. Aborting." >&2
    exit 1
fi


if [ ! -f ".git_myconfig" ]; then
    while true; do
        read -pr -n 1 "Current directory does not have a configuration file. Create one? [y/n] " ans
        case "$ans" in
            y|Y)
                take_input 
                break
                ;;
            n|N)
                echo "No idea what to do."
                exit 1
                ;;
            *)
                echo "Enter y or n."
                
        esac
    done
fi



case "$#" in
    0)
        echo "Called with no args"
        ;;
    1)
        echo "Called with 1 arg"
        ;;
    2)
        echo "Called with 2 args"
        ;;
    *)
        echo "Too many args"
        ;;
esac

