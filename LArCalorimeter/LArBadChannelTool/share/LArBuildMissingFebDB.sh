#!/bin/bash

if [[ $# < 4 ]];
then
  echo "Syntax: $0 [-append] [-offline] [-onerun] <Run> <LBb> <LBe> File1 File2 ..."
  exit
fi

inputTextFile="mf_input.txt"
outputTextFile="mf_output.txt"
outputSqlite="MissingFEBs.db"
outputSqliteOnl="MissingFEBsOnl.db"
oldTextFile="mf_previous.txt"
diffTextFile="mf_diff.txt"


if [ $1 == "-append" ]
then
    echo "Appending to previous bad-FEB list"
    catfiles=$oldTextFile
    shift
else
    catfiles=""
fi

if [ $1 == "-offline" ]
then
    echo "Creating only offline tags"
    online=0
    shift
else
    online=1 
fi

if [ $1 == "-onerun" ]
then
    echo "Creating closed IOV, for one run only"
    onerun=1
    shift
else
    onerun=0 
fi

if echo $1 | grep -q "^[0-9]\{5,\}$";
then
    runnumber=$1
    shift
else
    echo "ERROR: Expected a run-number, got $1"
    exit
fi


if echo $1 | grep -q "^[0-9]*$";
then
    lbnumber=$1
    shift
else
    echo "ERROR: Expected a lumi-block-number, got $1"
    exit
fi

if [ $onerun == 1 ]
then
  if echo $1 | grep -q "^[0-9]*$";
  then
    lbnumbere=$1
    shift
  else
    echo "ERROR: Expected a lumi-block-number, got $1"
    exit
  fi
fi

if ! which AtlCoolCopy.exe 1>/dev/null 2>&1
then
    echo "No offline setup found!"
    exit
fi

if [ -f $inputTextFile ];
then
  echo "Temporary file $inputTextFile exists already. Please remove!"
  exit
fi

if [ -f $outputTextFile ];
then
  echo "Temporary file $outputTextFile exists already. Please remove!"
  exit
fi

if [ -f $outputSqlite ];
then
  echo "Output file $outputSqlite exists already. Please remove!"
  exit
fi

if [ -f $outputSqliteOnl ];
then
  echo "Output file $outputSqliteOnl exists already. Please remove!"
  exit
fi

if [ -f $oldTextFile ];
then
  echo "Output file $oldTextFile exists already. Please remove!"
  exit
fi


if [ -f ${outputSqlite}.tmp ];
then
    rm -f ${outputSqlite}.tmp
fi

while [[ $# >0 ]]
do
  if [ ! -f $1 ];
      then
      echo "ERROR File $1 not found!"
      exit
  fi
  echo "Adding $1"
  catfiles="${catfiles} $1"
  shift
done


#Get UPD4-nn tag connected to 'current':
echo "Resolving current folder-level tag suffix for /LAR/BadChannelsOfl/MissingFEBs...."
fulltag=`getCurrentFolderTag.py "COOLOFL_LAR/CONDBR2" /LAR/BadChannelsOfl/MissingFEBs` 
upd4TagName=`echo $fulltag | grep -o "RUN2-UPD4-[0-9][0-9]"` 
echo "Found $upd4TagName"




echo "Running athena to read current database content..."
athena.py -c "OutputFile=\"${oldTextFile}\"" LArBadChannelTool/LArMissingFebs2Ascii.py > oracle2ascii.log 2>&1
if [ $? -ne 0 ];  then
    echo "Athena reported an error reading back sqlite file ! Please check oracle2ascii.log!"
    exit
fi


cat $catfiles > $inputTextFile
if [ $? -ne 0 ];  then
    echo "Failed to concatinate input files!"
    exit
fi

if [ $onerun -eq 1 ]; then
 #prefix=$[ $runnumber + 1]
 #prefix="IOVEndRun=${prefix};IOVEndLB=0;"
 endlb=$[ $lbnumbere + 1]
 prefix="IOVEndRun=${runnumber};IOVEndLB=$endlb;"
else 
 prefix=""
fi
echo "Running athena to build sqlite database file ..."
prefix="${prefix}IOVBeginRun=${runnumber};IOVBeginLB=${lbnumber};sqlite=\"${outputSqlite}.tmp\""
athena.py -c $prefix LArBadChannelTool/LArMissingFebDbAlg.py > ascii2sqlite.log 2>&1

if [ $? -ne 0 ];  then
    echo "Athena reported an error! Please check ascii2sqlite.log!"
    exit
fi

 
if grep -q ERROR ascii2sqlite.log
then
    echo "An error occured during ascii2sqlite job! Please check ascii2sqlite.log!"
    exit
fi

if grep -q "REJECTED" ascii2sqlite.log
then
    echo "ERROR: At least one line in the input text file could not be read. Syntax Error?"
fi


cp ${outputSqlite}.tmp ${outputSqlite}

if [ $onerun -eq 1 ]; then
   pref="RunNumber=${runnumber};LBNumber=${lbnumber};"
else   
   pref=""
fi
pref="${pref}sqlite=\"${outputSqlite}\";OutputFile=\"${outputTextFile}\""
echo "Running athena to test readback of sqlite database file"
athena.py  -c ${pref} LArBadChannelTool/LArMissingFebs2Ascii.py > sqlite2ascii.log 2>&1

if [ $? -ne 0 ];  then
    echo "Athena reported an error reading back sqlite file ! Please check sqlite2ascii.log!"
    exit
fi


if grep -q ERROR sqlite2ascii.log
then
    echo "An error occured during reading back sqlite file ! Please check sqlite2ascii.log!"
    exit
fi

if [ $online -eq 1 ]; then
   echo "Copying UPD3 to UPD1 tag..."
   AtlCoolCopy.exe "sqlite://;schema=${outputSqlite}.tmp;dbname=CONDBR2" "sqlite://;schema=${outputSqlite};dbname=CONDBR2" -f /LAR/BadChannelsOfl/MissingFEBs -t LARBadChannelsOflMissingFEBs-RUN2-UPD3-01 -ot LARBadChannelsOflMissingFEBs-RUN2-UPD1-01 -r 2147483647 -a  > AtlCoolCopy.upd3.log 2>&1

   if [ $? -ne 0 ];  then
       echo "AtlCoolCopy reported an error! Please check AtlCoolCopy.upd3.log!"
       exit
   fi
fi

echo "Copying UPD3 to UPD4 tag..."
AtlCoolCopy.exe "sqlite://;schema=${outputSqlite}.tmp;dbname=CONDBR2" "sqlite://;schema=${outputSqlite};dbname=CONDBR2" -f /LAR/BadChannelsOfl/MissingFEBs -t LARBadChannelsOflMissingFEBs-RUN2-UPD3-01 -ot LARBadChannelsOflMissingFEBs-$upd4TagName  > AtlCoolCopy.upd4.log 2>&1

if [ $? -ne 0 ];  then
    echo "AtlCoolCopy reported an error! Please check AtlCoolCopy.upd4.log!"
    exit
fi

if ! grep -q ${runnumber} /afs/cern.ch/user/a/atlcond/scratch0/nemo/prod/web/calibruns.txt
    then
    echo " *** WARNING *** Run ${runnumber} is not on the NEMO watchlist! Outside of CalibLoop? Not a stable-beam run?" 
fi


if [ $online -eq 1 ]; then
   echo "Copying to for online database..."
   AtlCoolCopy.exe "sqlite://;schema=${outputSqlite}.tmp;dbname=CONDBR2" "sqlite://;schema=${outputSqliteOnl};dbname=CONDBR2" -f /LAR/BadChannelsOfl/MissingFEBs -t LARBadChannelsOflMissingFEBs-RUN2-UPD3-01 -of  /LAR/BadChannels/MissingFEBs -ot LARBadChannelsMissingFEBs-RUN2-UPD1-01 -r 2147483647 -a -c > AtlCoolCopy.onl.log 2>&1


   if [ $? -ne 0 ];  then
      echo "AtlCoolCopy reported an error! Please check AtlCoolCopy.onl.log!"
      exit
   fi
fi

if [ -f $diffTextFile ]
then 
    rm -rf $diffTextFile
fi

diff $oldTextFile $outputTextFile > $diffTextFile

nNew=`grep -c ">" $diffTextFile`
nGone=`grep -c "<" $diffTextFile`
nTotal=`wc -l $outputTextFile | cut -f 1 -d " "`

echo "Added $nNew missing FEBs and removed $nGone"
echo "Total number of FEBs in the new list: $nTotal" 


echo "Output files:"
echo "$outputSqlite: Containing UPD3 version of the missing FEB list for the OFFLINE DB. UPD3 valid as of run $runnumber"
if [ $online -eq 1 ]; then
   echo "            and UPD1 for the OFFLINE DB"
   echo "$outputSqliteOnl: Containing UPD1 version of the missing FEB list for the ONLINE DB"
fi   
echo "$outputTextFile: Text version of the new bad channel list (read back from sqlite)"

echo "" 
echo "Upload to OFFLINE oracle server using"
echo "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py ${outputSqlite} CONDBR2 ATLAS_COOLWRITE ATLAS_COOLOFL_LAR_W <password>"
echo ""
if [ $online -eq 1 ]; then
  echo "Upload to ONLINE oracle server using"
  echo "/afs/cern.ch/user/a/atlcond/utils/AtlCoolMerge.py --online ${outputSqliteOnl} CONDBR2 ATONR_COOL ATLAS_COOLONL_LAR_W <password>"
fi


