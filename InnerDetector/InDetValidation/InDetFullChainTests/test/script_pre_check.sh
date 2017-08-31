#
echo "Starting script_pre_check.sh"
#
echo in `pwd`
#
#Cleaning
echo "## cleaning"
#
/bin/rm -f *stripped
#/bin/rm -f InDetIBL_ExampleTesting*.log
/bin/rm -f geomDB
/bin/rm -f sqlite200
/bin/rm -f triggerDB
#echo "## setting up run time files"

#
# Local DB 
#
echo "## setting up local DB access"
#setupLocalDBReplica_CERN.sh
echo Setting up geomDB and triggerDB links in current directory to use 
echo CERN afs SQLite replicas of geometry and trigger databases
if [ ! -e geomDB ]
then
    ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB geomDB
fi
if [ ! -e triggerDB ]
then
    ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/triggerDB triggerDB
fi
# end setting up local DB access

#
#Set All
#
