#!/bin/bash

if [[ $# < 4 ]];
then
    echo "Syntax: $0 <Run1> <LB1>  <Run2> <LB2> <Run3> <LB3>"
    echo "<Run1> <LB1> : run /lumiblock to query HV database to check new corrections "
    echo "<Run2> <LB2> : run /lumiblock for IOV start of database update"
    echo "<Run3> <LB3> : run /lumiblock for IOV end of database update (if not given use open-ended IoV)"
    exit
fi

summaryFile="noise.summary.txt"

run1=$1
lb1=$2
run2=$3
lb2=$4

run3=-1
lb3=-1

if [[ $# >4 ]];
then
 run3=$5
 lb3=$6
fi

echo " Run /LB to compute new HV corrections " ${run1} ${lb1}
echo " Run /LB for IoOV start update for noise     " ${run2} ${lb2}
if [[ $run3 > 0 ]];
then
 echo " Run /LB for IoOV end update for noise     " ${run3} ${lb3}
fi

echo "    be careful: you need ~60 Mbytes of free disk space in the running directory... "

if [ -f larnoisesqlite.db ];
    then
    echo "ERROR: Output file larnoisesqlite.db exists already. Remove it before running this script !"
    exit
fi

if [ -f $summaryFile ]
    then 
    /bin/rm  $summaryFile
fi

if [ -f calonoise.txt ]
    then
    /bin/rm calonoise.txt
fi

if [ -f cellnoise_data.root ]
    then
    /bin/rm cellnoise_data.root
fi

touch $summaryFile

if ! which AtlCoolCopy.exe 1>/dev/null 2>&1
then
    echo "No offline setup found!"
    exit
fi

cat > getGlobalESTag.py << _EOF4_
import sys
from PyCool import cool
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias
resolver=resolveAlias()
currentGlobal=resolver.getCurrentES().replace("*","ST")
print currentGlobal
_EOF4_

globalTag=`python getGlobalESTag.py | awk '{print($1)}'`

#globalTag="COMCOND-ES1PA-006-01"

echo " ES1 global DB tag " $globalTag


echo "Running athena to compute rescaled noise values from new HV corrections"
athena.py -c "RunNumber=${run1};LumiBlock=${lb1};GlobalTag=\"${globalTag}\""  CaloCondPhysAlgs/CaloRescaleNoise_offlineOnly_jobOptions.py > noise.log 2>&1
if [ $? -ne 0 ];  then
      echo "Athena reported an error ! Please check noise.log!"
      exit
fi

if grep -q ERROR noise.log
      then
      echo "An error occured ! Please check noise.log!"
      exit
fi

nLines=`wc -l calonoise.txt | cut -f 1 -d " "`
echo " Ncells*Ngains for which noise is computed  " ${nLines}
if [ $nLines -ne 547404 ]; then
     echo "Number of lines in noise ascii file is not correct, check log file to investigate possible problem (or disk space)"
     exit
fi

cat > noiseCheck.C << _EOF2_
{
 TFile f1("cellnoise_data.root");
 TTree* t1 = f1.Get("calonoise/mytree");
 t1->SetScanField(99999999);
 ((TTreePlayer*)(t1->GetPlayer()))->SetScanRedirect(true); 
 ((TTreePlayer*)(t1->GetPlayer()))->SetScanFileName("noise.txt");
 t1->Scan("layer:eta:phi:ElecNoise:ElecNoiseRescaled","abs(ElecNoise-ElecNoiseRescaled)/ElecNoise >0.02 && iCool<40 && iGain==0"); 
}
_EOF2_

root -b -l -n -q noiseCheck.C > readroot.log 2>&1
nCell=`grep "selected entries" readroot.log | awk '{print($2)}'`
echo " -----> Number of channels which changed HV correction more than 2\%: " ${nCell}
echo "        Look at the file noise.txt for the full list of these channels"


echo "Resolving current folder-level tag suffix for /LAR/NoiseOfl/CellNoise"
fulltag=`getCurrentFolderTag.py "COOLOFL_LAR/CONDBR2"  /LAR/NoiseOfl/CellNoise | tail -1` 
#upd4TagName=`echo $fulltag | grep -o "UPD4-[0-9][0-9]"` 
#echo "Found $upd4TagName"


echo "Produce sqlite file for LAr noise values" 
CaloNoise_fillDB.py ${run2} ${lb2} ${run3} ${lb3} ${fulltag} larnoisesqlite.db > makedb.log 2>&1
if [ $? -ne 0 ];  then
    echo "An error is reported, check makedb.log"
    exit
fi

cat >> dumpDB.py << _EOF_
from CoolConvUtilities import AtlCoolTool
toolNew = AtlCoolTool.AtlCoolTool("sqlite://;schema=larnoisesqlite.db;dbname=COMP200")
toolNew.usetag("$fulltag")
a=toolNew.more('/LAR/NoiseOfl/CellNoise')
f = file('dumpDB.txt','w')
f.write(str(a))
f.close()
exit()
_EOF_

if [ -f dumpDB.txt ]
    then
     /bin/rm dumpDB.txt
fi

python dumpDB.py

echo " "
echo "   Content of new sqlite file : "
cat dumpDB.txt
echo "  "

if ! grep -q ${run1} /afs/cern.ch/user/a/atlcond/scratch0/nemo/prod/web/calibruns.txt
    then
    echo " *** WARNING *** Run ${run1} is not on the NEMO watchlist! Outside of CalibLoop? No stable beam?" 
elif ! grep -q ${run2} /afs/cern.ch/user/a/atlcond/scratch0/nemo/prod/web/calibruns.txt
    then
    echo " *** WARNING *** Run ${run2} is not on the NEMO watchlist! Outside of CalibLoop? No stable beam?" 
fi

if [[ $run3 > 0 ]];
    then
    if ! grep -q ${run3} /afs/cern.ch/user/a/atlcond/scratch0/nemo/prod/web/calibruns.txt
	then
	echo " *** WARNING *** Run ${run3} is not on the NEMO watchlist! Outside of CalibLoop? No stable beam?" 
    fi
fi


echo "  "
echo "  After checking that everything is OK, upload the new sqlite file with:"
echo "  /afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --nobackup larnoisesqlite.db  CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_LAR_W <password>"
exit
