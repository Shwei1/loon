#!/usr/bin/env bash

print_help() {
    local help_msg="Script used for initializing and configuring new git repositories.
Usage: ./git_init.sh [LOCAL_REPO_NAME] [REMOTE_REPO_URL]

"
    echo "$help_msg"
    exit 0
}

process_user_info() {
    read -pr "username: " USER_NAME
    read -pr "email: " USER_EMAIL
    read -pr "default branch name: " USER_BRANCH

    touch ./.git_myconfig
    {
        echo "USER_NAME=${USER_NAME}";
        echo "USER_EMAIL=${USER_EMAIL}";
        echo "USER_BRANCH=${USER_BRANCH}";
    } >> ./.git_myconfig

    export USER_NAME USER_EMAIL USER_BRANCH
}

check_if_empty_or_repo() {
    if [ -d "$1/.git" ] || [ -n "$(find "$1" -maxdepth 1 -mindepth 1)" ]; then
        return 1
    else
        return 0
    fi
}

check_if_config_exists() {
    if [ ! -f ".git_myconfig" ] || [ ! -n "$USER_NAME" ] || [ ! -n "$USER_EMAIL" ] || [ ! -n "$USER_BRANCH" ]; then
        while true; do
            read -pr -n 1 "Current directory does not have a configuration file. Create one? [y/n] " ans
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
    git config --local user.name "$USER_NAME"
    git config --local user.email "$USER_EMAIL"
    git config --local init.defaultBranch "$USER_BRANCH"
}

create_local_repo() {
    git init "$1"
    configure_local_repo
    echo "# $1" > "$1"/README.md
}

link_remote_repo() {
    git remote add origin "$1"
}

case "$#" in
    0)
        #echo "Called with no args"
        print_help
        ;;
    1)
        #echo "Called with 1 arg"
        if check_if_empty_or_repo "$1"; then
            echo "Given directory is already a git repository"
        fi
        check_if_config_exists
        create_local_repo "$1"
        ;;
    2)
        #echo "Called with 2 args"
        check_if_config_exists
        create_local_repo "$1"
        link_remote_repo "$2"
        ;;
    *)
        echo "Too many args"
        ;;
esac

