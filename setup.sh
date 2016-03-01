#!/bin/sh

# Remove the remote (you probably want your own instead).
git remote remove origin

# Revert to first commit, add and commit everything as single commit.
git reset "$(git rev-list --max-parents=0 --abbrev-commit HEAD)"

git add --all

git commit --amend  # Will prompt for message.
