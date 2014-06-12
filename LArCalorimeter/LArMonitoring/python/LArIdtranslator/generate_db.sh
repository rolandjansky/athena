#!/bin/sh
export installPath=/afs/cern.ch/user/l/larmon/public/LArIdtranslator/LArIdtranslator_17.1.0/
cd $installPath
#source setup.sh
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
source $AtlasSetup/scripts/asetup.sh 17.2.9.1,32bit,opt 

cd ./LArIdtranslator/run/
rm -f usedTags.txt
rm -f LArId.out
rm -f LArBad.out

echo " "
echo " ------ Will create the LarId.db ------ "
echo " BT - July 13: this step does not work after slc6 migration"
echo " No problem as long as no change in mapping, I guess"
python ./create_LArId.py > LArId.out
mv ./LArId_tmp.db /afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/LArId.db

echo " "
echo " "
echo " "
echo " ------ Will create the LarBad.db ------ "
echo " "
python ./create_LArBad.py > LArBad.out
mv ./LArBad_tmp.db /afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/LArBad.db

grep 'Tag used for LAr' *.out | awk -F ":" '{print $2}' > usedTags.txt
mv ./usedTags.txt /afs/cern.ch/user/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/usedTags.txt
