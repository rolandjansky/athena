#!/bin/sh
if [ $# -ne 1 ]; then
echo "atlasjobs/test_relname: one argument is required - release name"
exit 9
fi

release="$1"; shift

echo $release | grep '[0-9]' > /dev/null 2>&1; stat0=$?
if [ "$stat0" = "1" ]; then exit 1; fi
echo $release | grep -v '[a-zA-Z]' > /dev/null 2>&1; stat0=$?
if [ "$stat0" = "1" ]; then exit 2; fi
relnum=`echo $release | sed 's/\.//g'`
echo $relnum | grep '[^0-9]' > /dev/null 2>&1; stat0=$?
if [ "$stat0" = "0" ]; then exit 2; fi
