#!/bin/sh
#
# Utility script to install a new svn version of the AtlDataSummary code
#
if [ $# -eq 0 ]
then
    echo "Enter version number (00-00-00):"
    read tag
else
    tag="$1"
fi
echo "Switching to CoolLumiUtilities-$tag"

cd Database/CoolLumiUtilities
# Use svn switch
 svn switch $SVNROOT/Database/CoolLumiUtilities/tags/CoolLumiUtilities-$tag
# Also write new version
cd ../..
echo "CoolLumiUtilities-$tag" > version.html
# all done
exit
