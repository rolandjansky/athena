#!/bin/sh
#
# Utility script to tag version of AtlDataSummary
#
if [ $# -eq 0 ]
then
    echo "Enter version number (00-00-00):"
    read tag
else
    tag="$1"
fi
svn cp . $SVNROOT/Database/CoolLumiUtilities/tags/CoolLumiUtilities-${tag}
exit
