#!/bin/bash

[[ $# -eq 0 ]] && { echo "Usage: ${0##*/} FILE ..." ; exit 0 ; }

DEF=$(mktemp -t || exit 1)
REQ=$(mktemp -t || exit 1)
trap "rm -f ${DEF} ${REQ}" HUP INT TERM EXIT

for i ; do
    echo "${0##*/}: ${i}: Checking for syntax errors ..."
    han-config-parse.awk -- "$i" 2>&1 || continue
    # if this already goes wrong then the rest does not make sense

    han-config-parse.awk DEF=1 "$i" > "${DEF}"
    han-config-parse.awk REQ=1 "$i" > "${REQ}"
    # cache the output in temporary files because we need the same thing three times

    echo "${0##*/}: ${i}: Checking for redefined objects ..."
    cat "${DEF}" | sort | uniq -d
    # print everything that appears more than once - only acceptable for "output"

    echo "${0##*/}: ${i}: Checking for undefined objects ..."
    join -v2 -t'#' <(cat "${DEF}" | sort -u) <(cat "${REQ}" | sort -u)
    # we want to compare entire lines, so we use "#" as the field separator (which will never appear)

    echo "${0##*/}: ${i}: Checking for unused objects ..."
    join -v1 -t'#' <(cat "${DEF}" | sort -u) <(cat "${REQ}" | awk '{ print; while (index($0, "/")) { sub("/[^/]*$", ""); print; } }' | sort -u)
    # the inline awk script transforms "a/b/c" into "a/b/c a/b a", which are all implicitly required
done
