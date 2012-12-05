#!/bin/bash

function usage()
{
    echo "Usage: ${0##*/} [IN.CONFIG [OUT.CONFIG]]"
    echo "       ${0##*/} -h | --help"
}

function help()
{
    usage
    echo
    echo "Adjusts the indentation of DQMF Offline (\"han\") configuration files."
    echo "Reads from a file or from stdin, writes to a file or to stdout."
}

[[ "$#" -gt 2 ]] && { usage 1>&2 ; exit 1 ; }
[[ "$1" == "-h" || "$1" == "-help" || "$1" == "--help" ]] && { help ; exit 0 ; }

INDENT="  "
PREFIX=""

sed 's/^[[:space:]]\+//;s/[[:space:]]\+$//' < "${1:-/dev/stdin}" | \
while read ; do

[[ "${REPLY}" == *}* && "${REPLY}" != \#* ]] && PREFIX="${PREFIX%$INDENT}"
[[ "${REPLY}" ]] && { echo "${PREFIX}${REPLY}" ; } || { echo ; }
[[ "${REPLY}" == *{* && "${REPLY}" != \#* ]] && PREFIX="${PREFIX}${INDENT}"

done > "${2:-/dev/stdout}"
