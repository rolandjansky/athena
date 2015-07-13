#!/bin/sh
cd /afs/cern.ch/work/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/run
export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
source $AtlasSetup/scripts/asetup.sh --input /afs/cern.ch/work/l/larmon/public/asetup 

/bin/rm -f usedTags.txt
/bin/rm -f LArId.out
/bin/rm -f LArBad.out
/bin/rm -f LArId_tmp.db
/bin/rm -f LArBad_tmp.db

echo " "
echo " ------ Will create the LarId.db ------ "
python ./create_LArId.py 2>&1 3>&1 > LArId.out
if [[ $? == "0" ]]
then
   /bin/mv -f ./LArId_tmp.db /afs/cern.ch/work/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/LArId.db
else   
   echo "LArId.db creation ERROR !!!"
fi

echo " "
echo " "
echo " "
echo " ------ Will create the LarBad.db ------ "
echo " "
python ./create_LArBad.py 2>&1 3>&1 > LArBad.out
if [[ $? == "0" ]]
then
  /bin/mv -f ./LArBad_tmp.db /afs/cern.ch/work/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/LArBad.db
  grep 'Tag used for LAr' *.out | awk -F ":" '{print $2}' > usedTags.txt
  /bin/mv -f ./usedTags.txt /afs/cern.ch/work/l/larmon/public/LArCalorimeter/LArMonitoring/python/LArIdtranslator/usedTags.txt
else
  echo "LArBad.db creation ERROR !!!"
fi
