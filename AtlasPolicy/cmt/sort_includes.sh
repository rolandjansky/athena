#!/bin/sh
# ========================================================================= #
#                                                                           #
#    sort_includes.sh [Arguments]                                           #
#                                                                           #
#    Shell script to sort include paths such that project InstallAreas are  #
#    correctly handled.                                                     #
#                                                                           #
#    Arguments:                                                             #
#        $1                                                #
#        $2      <package>_root                                             #
#                                                                           #
# ========================================================================= #

#+
# Main Program
#-

# set -x

list=

# --- Default parameters

while [ $# -ne 0 ]; do
    case $1 in
        */InstallArea/*) list="$1 $list";;
        *)               list="$list $1";;
    esac
    shift
done
echo $list
exit 0
