#
echo "Starting script_pre_check.sh"
#
cd ${ATN_PACKAGE}/run
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
echo Setting up geomDB, triggerDB and sqlite200 links in current directory to use 
echo CERN afs SQLite replicas of geometry, conditions and trigger databases
if [ ! -e geomDB ]
then
    ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB geomDB
fi
if [ ! -e triggerDB ]
then
    ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/triggerDB triggerDB
fi
if [ ! -e sqlite200 ]
then
    if [ -n "$1" -a "$1" = "COMP200" ]
    then
	echo Special setup for COMP200 replica
	mkdir sqlite200
	ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db sqlite200/ALLP200.db
    else
	ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200 sqlite200
    fi
fi
# end setting up local DB access

#
#Set All
#
echo "## setting up environment "
source ../cmt/setup.sh
echo "## ready to run"
