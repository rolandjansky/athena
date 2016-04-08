if [ -e sqlite200 ]
then
    rm -r sqlite200
fi

ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB .
ln -s /afs/cern.ch/atlas/conditions/poolcond/catalogue/poolcond .

export STAGE_HOST=castoratlas 
export RFIO_USE_CASTOR_V2=YES 
export STAGE_SVCCLASS=atlcal

