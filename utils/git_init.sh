#!/usr/bin/env bash

print_help() {
    local help_msg="Script used for initializing and configuring new git repositories.
Usage: ./git_init.sh [LOCAL_REPO_NAME] [REMOTE_REPO_URL]"
    echo "$help_msg"
    exit 0
}

process_user_info() {
    read -rp "username: " USER_NAME
    read -rp "email: " USER_EMAIL
    read -rp "default branch name: " USER_BRANCH

    touch ./.git_myconfig
    {
        echo "USER_NAME=${USER_NAME}";
        echo "USER_EMAIL=${USER_EMAIL}";
        echo "USER_BRANCH=${USER_BRANCH}";
    } >> ./.git_myconfig

    export USER_NAME USER_EMAIL USER_BRANCH
}

check_if_nonempty() {
    [ -d "$1" ] && [ -n "$(find "$1" -maxdepth 1 -mindepth 1)" ]
}

check_if_repo() {
    [ -d "$1/.git" ]
}

load_config() {
    if [ ! -f ".git_myconfig" ]; then
        while true; do
            read -n 1 -rp "Current directory does not have a configuration file. Create one? [y/n] " ans
            echo
            case "$ans" in
                y|Y)
                    process_user_info
                    return
                    ;;
                n|N)
                    echo "No idea what to do."
                    exit 1
                    ;;
                *)
                    echo "Enter y or n."
                    ;;
            esac
        done
    fi
    . ./.git_myconfig
}

configure_local_repo() {
    git -C "$1" config --local user.name "$USER_NAME"
    git -C "$1" config --local user.email "$USER_EMAIL"
    git -C "$1" config --local init.defaultBranch "$USER_BRANCH"
}

create_local_repo() {
    git init "$1"
    configure_local_repo "$1"
    echo "# $1" > "$1"/README.md
}

link_remote_repo() {
    git -C "$1" remote add origin "$2"
}

setup_local_repo() {
    if check_if_nonempty "$1"; then
        echo "Given directory exists and is nonempty. Aborting." >&2
        exit 1
    fi
    load_config
    create_local_repo "$1"
}


case "$#" in
    0)
        #echo "Called with no args"
        print_help
        ;;
    1)
        #echo "Called with 1 arg"
        if check_if_repo "."; then
            echo "Current directory is a git repository. Aborting" >&2
            exit 1
        fi
        setup_local_repo "$1"
        ;;
    2)
        #echo "Called with 2 args"
        if check_if_repo "."; then
            echo "Current directory is a git repository. Aborting" >&2
            exit 1
        fi
        if check_if_repo "$1"; then
            link_remote_repo "$1" "$2"
        else
            setup_local_repo "$1"
            link_remote_repo "$1" "$2"
        fi
        ;;
    *)
        echo "Too many args"
        exit 1
        ;;
esac

