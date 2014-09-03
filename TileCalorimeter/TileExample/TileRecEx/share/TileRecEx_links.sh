if [ -e sqlite200 ]
then
    rm -rf sqlite200
fi
mkdir sqlite200
cd sqlite200
if [ -e ALLP200.db ] 
then 
    rm -f ALLP200.db
fi 
if [ -e COMP200.db ]
then
    rm -f COMP200.db
fi


ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db ALLP200.db
ln -s /afs/cern.ch/user/a/atlcond/coolrep/sqlite200/COMP200.db COMP200.db

cd ..

if [ ! -e geomDB ]
then 
     ln -s /afs/cern.ch/atlas/project/database/DBREL/packaging/DBRelease/current/geomDB .
fi

#export STAGE_HOST=castoratlas 
#export RFIO_USE_CASTOR_V2=YES 
#export STAGE_SVCCLASS=t0atlas

