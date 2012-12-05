#!/bin/bash

MERGEALL="merge_all_han_configs.sh"

function usage()
{
    echo "Usage: ${0##*/} DIR..."
    echo "       ${0##*/} --help"
}

function help()
{
    usage
    echo
    echo "Runs ${MERGEALL}, including only the configurations from"
    echo "the subdirectories given on the command line (and \"common\")."
}

[[ $# -eq 0 ]] && { usage 1>&2 ; exit 1 ; }
[[ "$1" == "-h" || "$1" == "--help" ]] && { help ; exit 0 ; }

INCLUDE=".svn:common"
for i in "$@" ; do
    [[ -d "${i}" ]] || { echo "${0##*/}: ${i}: No such directory" 1>&2 ; exit 1 ; }
    [[ "${i}" == */* ]] && { echo "${0##*/}: ${i}: Illegal argument: Pathnames not allowed" 1>&2 ; exit 1 ; }
    INCLUDE="${INCLUDE}:${i}"
done
INCLUDE="$(tr ":" "\n" <<< "${INCLUDE}")"

find . -mindepth 1 -maxdepth 1 -type d -printf "--exclude %f\n" | sort | grep -vwF "$INCLUDE" | xargs -t "${MERGEALL}"
