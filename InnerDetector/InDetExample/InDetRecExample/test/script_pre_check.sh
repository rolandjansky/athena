#
echo "##"
echo "##"
echo "##"
echo "Starting script_pre_check.sh"
echo "##"
echo "##"
echo "##"
#

cd ../run
echo in `pwd`

#
#Cleaning
echo "##"
echo "##"
echo "##"
echo "## cleaning"
echo "##"
echo "##"
echo "##"
/bin/rm -f *stripped
/bin/rm -f InDetRecExampleTesting.log
/bin/rm -f geomDB
/bin/rm -f sqlite200
#echo "## setting up run time files"

#
# Local DB 
#
echo "##"
echo "##"
echo "##"
echo "## setting up local DB access"
echo "##"
echo "##"
echo "##"
setupLocalDBReplica_CERN.sh

#
#Set All
#
echo "##"
echo "##"
echo "##"
echo "## setting up environment "
echo "##"
echo "##"
echo "##"
source ../cmt/setup.sh
echo "##"
echo "##"
echo "##"
echo "## ready to run"
echo "##"
echo "##"
echo "##"
