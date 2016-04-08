#!/bin/csh

# should eventually not be necessary
set echo
#get_files -symlink -jo AthenaCommon/athena.py
ln -sf /afs/cern.ch/atlas/user/a/atlcond/coolrep/sqlite200 .

# copy example jobOptions
cp ../share/jobOptions.py ../run/jobOptions.py

# copy example PoolFileCatalog
cp ../share/PoolFileCatalog.xml ../run/PoolFileCatalog.xml

# and class to display histograms using root
cp ../share/rootHistogs.cxx ../run/rootHistogs.cxx

