#!/bin/sh

if [ -z $SED_PROGRAM ]; then
    echo "SED_PROGRAM is not set"
    exit 1
fi

beautify()
{
    while read i; do
        echo -n $i | $SED_PROGRAM -e 's,^, \\\n\t,'
    done
    echo
}

echo -n "libFileStager_la_SOURCES ="
ls -1 Root/T*.cxx | beautify
echo
echo -n "nobase_include_HEADERS ="
ls -1 FileStager/T*.h | beautify
echo
echo -n "StageMonitor_SOURCES ="
ls -1 bin/*.cxx | beautify
echo
