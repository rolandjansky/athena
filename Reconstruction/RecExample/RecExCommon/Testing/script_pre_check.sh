#
 echo "Starting script_pre_check.sh"
#
#Cleaning

echo "## cleaning"
 /bin/rm -f *stripped
 /bin/rm -f RecExCommonTesting.log


echo "## setting up run time files"
 RecExCommon_links.sh

#

echo "## ready to run"

env > enviroment.log
