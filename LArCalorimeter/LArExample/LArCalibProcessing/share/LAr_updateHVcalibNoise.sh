#!/bin/bash

if [[ $# < 3 ]];
then
    echo "Syntax: $0 <time>  <Run> <LB> [mu] [dt]"
    echo " <time> = time in UTC at which the HV is stable at the new conditions (and HV mapping updated if needed) like \"2010-07-29:13:00:00\" "
    echo " <Run> <LB> = run/lumiblock to start IoV for UPD4 noise update (allows to backdate)"
    echo " [mu] [dt] are values of mu and bunch spacing for which we compute noise, if not"
    echo "           given, the current values will be used"
    exit
fi


echo " -----"
echo "This script will compute the HV scale correction for the given time (UTC) " 
echo "    the new HV setting should be correct / stable for this Time "
echo "    in case of HV mapping change, the new mapping should be valid (in the UPD4 tag) for this specified time (be careful of glitch at the mapping db is in run/lb IoV) "
echo " "
echo "It will also produce new noise tables for online/offline database"
echo " "


time=$1
run=$2
lb=$3
mu=-1
dt=-1

if [[ $# > 3 ]];
then
 mu=$4
fi

if [[ $# > 4 ]];
then
 dt=$5
fi

echo " $0 $1 $2 $3 $mu $dt"

summaryFile=noise_summary.txt

echo " Time to look at HV situation                           " ${time}
echo " Run /LB for IoOV start update for noise   in UPD4 tag  " ${run} ${lb}

echo "    be careful: you need ~60 Mbytes of free disk space in the running directory... "

if [ -f HVScaleCorr.db ];
   then
   echo "ERROR: local sqlite file for HV scale correction already exists. Remove  HVScaleCorr.db before running this script !"
   exit
fi

if [ -f larnoisesqlite.db ];
    then
    echo "ERROR: Output file larnoisesqlite.db exists already. Remove it before running this script !"
    exit
fi

if [ -f caloSqlite_UPD1_offline.db ];
    then
    /bin/rm caloSqlite_UPD1_offline.db
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

if [ -f dumpMapping.txt ]
    then
    /bin/rm dumpMapping.txt
fi

touch $summaryFile

if ! which AtlCoolCopy.exe 1>/dev/null 2>&1
then
    echo "No offline setup found!"
    exit
fi

cat > getGlobalTag.py << _EOF4_
import sys
from PyCool import cool
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias
resolver=resolveAlias()
currentGlobal=resolver.getCurrent().replace("*","ST")
print currentGlobal
_EOF4_

globalTag=`python getGlobalTag.py | awk '{print($1)}'`
#globalTag="COMCOND-BLKPA-006-01"

echo "Found current global tag $globalTag"



echo "Running athena to compute new HV corrections"
athena.py -c "date=\"${time}\";GlobalTag=\"${globalTag}\""  LArCalibProcessing/LArCalib_Example_HVCorr.py > hv.log 2>&1
if [ $? -ne 0 ];  then
      echo "Athena reported an error ! Please check hv.log!"
      exit
fi

if grep -q ERROR hv.log
      then
      echo "An error occured ! Please check hv.log!"
      exit
fi

usedRun=`grep "Working on run" hv.log | awk '{print($5)}'`
echo " ---> the run number to read the HV mapping db in the UPD4 tag is " ${usedRun}
grep "^--->" hv.log

cat > dumpMapping.py << _EOF1_
from CoolConvUtilities import AtlCoolTool
toolNew = AtlCoolTool.AtlCoolTool("COOLOFL_LAR/CONDBR2")
toolNew.usetag("${globalTag}")
a=toolNew.more("/LAR/IdentifierOfl/HVLineToElectrodeMap")
f=file('dumpMapping.txt','w')
f.write(str(a))
f.close()
exit()
_EOF1_

python dumpMapping.py >& python.log
runHVmap=`grep LArHVMap dumpMapping.txt  | tail -1 | cut -f 1  -d , | cut -f 2 -d \[`

echo "      the latest HV mapping change is valid from run " ${runHVmap}

if test ${runHVmap} -gt ${usedRun}; then
   echo "   BE CAREFUL  the used run is older than the latest HV mapping change... Check that you know what you are doing before any db upload "
fi




echo " "
echo "Running athena to read current HV scale factor correction in database for comparison"
athena.py -c "date=\"${time}\"" LArCalibProcessing/LArHVCool2Ntuple_jobOptions.py > readhv.log 2>&1
if [ $? -ne 0 ];  then
      echo "Athena reported an error ! Please check readhv.log!"
      exit
fi

if grep -q ERROR readhv.log
      then
      echo "An error occured ! Please check readhv.log!"
      exit
fi

cat > hvcheck.C << _EOF2_
{
 TFile f1("hvcorr_read.root");
 TFile f2("hvcorr_ntuple.root");
 TTree* t1 = (TTree*)f1.Get("HVSCALE");
 TTree* t2 = (TTree*)f2.Get("HVSCALE");
 t2->AddFriend(t1,"t1");
 t2->SetScanField(99999999);
 ((TTreePlayer*)(t2->GetPlayer()))->SetScanRedirect(true); 
 ((TTreePlayer*)(t2->GetPlayer()))->SetScanFileName("hvlist.txt");
 t2->Scan("barrel_ec:pos_neg:FT:layer:ieta:iphi:hvcorr:t1.hvcorr","abs(hvcorr-t1.hvcorr)>0.005"); 
}
_EOF2_

root -b -l -n -q hvcheck.C > readroot.log 2>&1
nHV=`grep "selected entries" readroot.log | awk '{print($2)}'`
echo " -----> Number of channels which changed HV correction more than 0.5\%: " ${nHV}
echo "        Look at the file hvlist.txt for the full list of these channels"

echo " "

# Now compute new L1Calo corrections based on new LArHV corr
cat > getGlobalTagES.py << _EOF5_
import sys
from PyCool import cool
sys.path.append('/afs/cern.ch/user/a/atlcond/utils/python/')
from AtlCoolBKLib import resolveAlias
resolver=resolveAlias()
currentGlobal=resolver.getCurrentES().replace("*","ST")
print currentGlobal
_EOF5_

globalTagES=`python getGlobalTagES.py | awk '{print($1)}'`
echo " "
echo "Running athena to compute L1Calo corrections"
athena.py -c "date=\"${time}\";GlobalTag=\"${globalTagES}\";RunNumber=$run;LumiBlock=$lb;HVCorrDb=\"sqlite://;schema=HVScaleCorr.db;dbname=CONDBR2\";OutputSQLiteFile=\"hvcorrections_${time%%:*}.sqlite\"" TrigT1CaloCalibUtils/LArL1Calo_DumpHVCorr.py > l1calocorr.log 2>&1
if [ $? -ne 0 ];  then
      echo "Athena reported an error ! Please check l1calocorr.log!"
      exit    
fi

if grep -q ERROR l1calocorr.log
      then    
      echo "An error occured ! Please check l1calocorr.log!"
      exit    
fi

# Now noise
echo " "
echo "Running athena to scale noise values from reference noise database"
echo "        asking mu=$mu and dt=$dt"
athena.py -c "mu=${mu};dt=$dt;date=\"${time}\";GlobalTag=\"${globalTag}\";sqliteHVCorr=\"HVScaleCorr.db\"" CaloCondPhysAlgs/CaloScaleNoise_jobOptions.py > noise.log 2>&1

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
echo "  Ncells*Ngains for which noise is computed  " ${nLines}
if [ $nLines -ne 547404 ]; then
     echo "Number of lines in noise ascii file is not correct, check log file to investigate possible problem or disk space"
     exit
fi

grep DifferentNoise noise.log > ${summaryFile}
nTotal=`wc -l ${summaryFile} | cut -f 1 -d " "`
echo " ---> Number of cells*Number of gains(3) for which noise changed more than 5\%" ${nTotal}
echo "      Check file "  ${summaryFile} " for the full list "
echo "      You can also look at cellnoise_data.root for ntuple information "

echo "Resolving current folder-level tag suffix for /LAR/NoiseOfl/CellNoise"
fulltag=`getCurrentFolderTag.py "COOLOFL_LAR/CONDBR2"  /LAR/NoiseOfl/CellNoise | tail -1` 
#upd4TagName=`echo $fulltag | grep -o "UPD4-[0-9][0-9]"` 
echo "Found $fulltag"


echo "Produce sqlite file for LAr noise values" 
CaloNoise_fillDB.py ${run} ${lb} -1 -1 ${fulltag} calonoise.txt larnoisesqlite.db > makedb.log 2>&1
if [ $? -ne 0 ];  then
    echo "An error is reported, check makedb.log"
    exit
fi

cat >> dumpDB.py << _EOF3_
from CoolConvUtilities import AtlCoolTool
toolNew = AtlCoolTool.AtlCoolTool("sqlite://;schema=larnoisesqlite.db;dbname=CONDBR2")
toolNew.usetag("$fulltag")
a=toolNew.more('/LAR/NoiseOfl/CellNoise')
f = file('dumpDB.txt','w')
f.write(str(a))
f.close()
exit()
_EOF3_

if [ -f dumpDB.txt ]
    then
     /bin/rm dumpDB.txt
fi

python dumpDB.py

echo " "
echo "   Content of new noise UPD4 sqlite file : "
cat dumpDB.txt

echo " " 

cp larnoisesqlite.db tempdb.db

echo "Make UPD1 offline folder for noise "
AtlCoolCopy.exe "sqlite://;schema=tempdb.db;dbname=CONDBR2" "sqlite://;schema=larnoisesqlite.db;dbname=CONDBR2" -f /LAR/NoiseOfl/CellNoise -t ${fulltag} -ot  LARNoiseOflCellNoise-RUN2-UPD1-00 -create >> mergedb.log 

/bin/rm tempdb.db

echo "Make UPD1 online folder for noise "
AtlCoolCopy.exe "sqlite://;schema=larnoisesqlite.db;dbname=CONDBR2" "sqlite://;schema=caloSqlite_UPD1_online.db;dbname=CONDBR2" -f /LAR/NoiseOfl/CellNoise -of /CALO/Noise/CellNoise -t ${fulltag} -ot  CaloNoiseCellnoise-RUN2-UPD1-00 -create >> mergedb.log 

echo "Doing check of the noise sqlite against P1HLT cache....."
echo "Will take 3-5 minutes, be patient......"
#(mkdir /tmp/noise_test_$$; cp caloSqlite_UPD1_online.db /tmp/noise_test_$$/; cd /tmp/noise_test_$$/; source $AtlasSetup/scripts/asetup.sh --tags=AtlasP1HLT,20.2.1.4,setup,here; athena.py -c "sqlite='caloSqlite_UPD1_online.db'" TriggerRelease/test_hltConditions.py >/dev/null 2>&1 ) >/dev/null 2>&1
(mkdir /tmp/noise_test_$$; cp caloSqlite_UPD1_online.db /tmp/noise_test_$$/; cd /tmp/noise_test_$$/; athena.py -c "sqlite='caloSqlite_UPD1_online.db'" TriggerRelease/test_hltConditions.py >/dev/null 2>&1 ) >/dev/null 2>&1

if [ $? -ne 0 ];  then
      echo "Testing job reported an error ! "
      echo "Please, do not upload constants to online ! "
      exit
fi

echo "  "
echo " mergedb.log contains the log file of the various AtlCoolCopy operation to make local sqlite files"
echo " " 
echo "  After checking that everything is OK you can proceed with the database update"
echo "  (1) ~atlcond/utils/AtlCoolMerge.py --online HVScaleCorr.db  CONDBR2 ATONR_COOL  ATLAS_COOLONL_LAR_W  <password>"
echo "  (2) ~atlcond/utils/AtlCoolMerge.py larnoisesqlite.db CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_LAR_W <password>"
echo "  (3) ~atlcond/utils/AtlCoolMerge.py --online caloSqlite_UPD1_online.db  CONDBR2 ATONR_COOL ATLAS_COOLONL_CALO_W <password>"

echo "  (note that password are different for LAr online,offline, Calo online offline databases"
exit
