#!/bin/sh

# should eventually not be necessary
#get_files -symlink -jo AthenaCommon/athena.py
#get_files -symlink -data Materials.dtd 
#get_files -symlink -data AGDD.dtd

#create symbolic links to get DB:
#ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite130 sqlite130
#ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB geomDB
#for rel 13
setupLocalDBReplica_CERN.sh

# get particle data table and magnetic field data
get_files -symlink -data PDGTABLE.MeV

# copy example jobOptions 
#cp ../share/HLT*xml ../run
#cp ../share/LVL1*xml ../run
#cp ../share/jobOptions.py ../run/
cp ../share/jobOptionsNewSteering.py ../run/
#cp ../share/PoolFileCatalog.xml ../run/
cp ../TESTS/test*py ../run/
