#
 echo "Starting script_pre_check.sh"
#

cd ../run
echo in `pwd`

#
#Cleaning
echo "## cleaning"
 /bin/rm -f *stripped
 /bin/rm -f RecExTBTesting.log
#echo "## setting up run time files"
 source ../share/RecExTB_links.sh
#
#Set All
#
echo "## setting up environment "
 source ../cmt/setup.sh
echo "## ready to run"
