#!/bin/sh
# Run this to generate all the initial makefiles, etc.

git log . > ChangeLog
touch NEWS README AUTHORS

which gnome-autogen.sh || {
    echo "You need to install gnome-common from the GNOME CVS"
    exit 1
}

. gnome-autogen.sh
