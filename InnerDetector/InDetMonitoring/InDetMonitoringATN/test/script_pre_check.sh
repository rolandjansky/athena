#
echo "Starting script_pre_check.sh"
#

cd ../run
echo in `pwd`

#
#Cleaning
echo "## cleaning"
/bin/rm -f *stripped
/bin/rm -f InDetMonitoringATN.log
/bin/rm -f geomDB
/bin/rm -f sqlite200

#
# Local DB 
#
#echo "## setting up local DB access"
#setupLocalDBReplica_CERN.sh

#
#Set All
#
echo "## setting up environment "
source ../cmt/setup.sh
echo "## ready to run"
