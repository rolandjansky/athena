#!/bin/bash

[[ $# -eq 0 ]] && { echo "Usage: ${0##*/} FILE ..." ; exit 0 ; }

DEF=$(mktemp -t || exit 1)
REQ=$(mktemp -t || exit 1)
trap "rm -f ${DEF} ${REQ}" HUP INT TERM EXIT
RV=0

echo "Testing han files for syntax and other problems."
echo "To reproduce locally run \"gmake test\" in your build directory."

# pick up the awk script installed with this bash script
AWKSCRIPT=$( dirname "${BASH_SOURCE[0]}" )/han-config-parse.awk
echo "Using awk script" $AWKSCRIPT

for i ; do
    echo "${0##*/}: ${i}: Checking for syntax errors ..."
    $AWKSCRIPT -- "$i" 2>&1
    if [ $? -ne 0 ]; then { RV=1 ; echo "not OK" ; continue ; } ; else echo "OK" ; fi 
    # if this already goes wrong then the rest does not make sense

    $AWKSCRIPT DEF=1 "$i" > "${DEF}"
    $AWKSCRIPT REQ=1 "$i" > "${REQ}"
    # cache the output in temporary files because we need the same thing three times

    echo "${0##*/}: ${i}: Checking for redefined objects that are not \"output\" blocks (having any is an error)..."
    cat "${DEF}" | sort | uniq -d | grep -v output
    # print everything that appears more than once - only acceptable for "output"
    # no output = grep exit code 1
    if [ $? -ne 1 ]; then { RV=1 ; echo "not OK" ; } ; else echo "OK" ; fi

    echo "${0##*/}: ${i}: Checking for objects that are referenced but not defined (having any is an error)..."
    UNDEF=$(join -v2 -t'#' <(cat "${DEF}" | sort -u) <(cat "${REQ}" | sort -u))
    echo "$UNDEF"
    # we want to compare entire lines, so we use "#" as the field separator (which will never appear)
    # any output is a failure
    if [ $(echo -n "$UNDEF" | wc -l) -ne "0" ]; then { RV=1 ; echo "not OK" ; } ; else echo "OK" ; fi

    echo "${0##*/}: ${i}: Checking for objects that are defined but not used (these are safe but clutter files)..."
    join -v1 -t'#' <(cat "${DEF}" | sort -u) <(cat "${REQ}" | awk '{ print; while (index($0, "/")) { sub("/[^/]*$", ""); print; } }' | sort -u)
    # the inline awk script transforms "a/b/c" into "a/b/c a/b a", which are all implicitly required
    # these are not really errors
    echo "done"
done

exit $RV
