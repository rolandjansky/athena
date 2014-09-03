#!/bin/tcsh
if ( ! -e sqlite200 )then
    mkdir sqlite200
endif

cd sqlite200
if (-e ALLP200.db ) then
    rm -f ALLP200.db
endif
if (-e COMP200.db ) then
    rm -f COMP200.db
endif

ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db ALLP200.db
ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db COMP200.db

cd ..

if ( ! -e sqlite200 )then
    ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB .
endif

#setenv STAGE_HOST castoratlas 
#setenv RFIO_USE_CASTOR_V2 YES 
#setenv STAGE_SVCCLASS t0atlas

