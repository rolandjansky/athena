#!/bin/tcsh
if ( -e sqlite200 )then
    rm -r sqlite200
endif

ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB .
ln -s /afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond .

setenv STAGE_HOST castoratlas 
setenv RFIO_USE_CASTOR_V2 YES 
setenv STAGE_SVCCLASS atlcal

