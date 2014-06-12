#!/bin/bash

if [[ $# < 3 ]];
then
    echo "Syntax: $0 <filename> <Run1> <LB1>  <Run2> <LB2>"
    echo "<Run1> <LB1> : run /lumiblock for IOV start of database update"
    echo "<Run2> <LB2> : run /lumiblock for IOV end of database update (if not given use open-ended IoV)"
    exit
fi

summaryFile="pedestal.summary.txt"

inputfilename=$1
run1=$2
lb1=$3
run2=-1
lb2=-1

if [[ $# >3 ]];
then
 run2=$4
 lb2=$5
fi

echo " Run /LB for IoOV start update for pedestal     " ${run1} ${lb1}
if [[ $run2 > 0 ]];
then
 echo " Run /LB for IoOV end update for pedestal      " ${run2} ${lb2}
fi

echo "    be careful: you need ~60 Mbytes of free disk space in the running directory... "

if [ -f caloSqlite.db ];
    then
    echo "ERROR: Output file caloSqlite exists already. Remove it before running this script !"
    exit
fi

if [ -f $summaryFile ]
    then 
    /bin/rm  $summaryFile
fi

if [ -f calopedestal.txt ]
    then
    /bin/rm calopedestal.txt
fi

if [ -f pedestalshift_data.root ]
    then
    /bin/rm pedestalshift_data.root
fi

touch $summaryFile

if ! which AtlCoolCopy.exe 1>/dev/null 2>&1
then
    echo "No offline setup found!"
    exit
fi

echo "Running athena to add pedestal shift values to existing UPD1 tag"
athena.py -c "FileName=\"${inputfilename}\";RunNumber=${run1};LumiBlock=${lb1}"  CaloCondPhysAlgs/CaloAddPedShift_jobOptions.py > pedestal.log 2>&1
if [ $? -ne 0 ];  then
      echo "Athena reported an error ! Please check pedestal.log!"
      exit
fi

if grep -q ERROR pedestal.log
      then
      echo "An error occured ! Please check pedestal.log!"
      exit
fi

nLines=`wc -l calopedestal.txt | cut -f 1 -d " "`
echo " Ncells*Ngains for which pedestal is computed  " ${nLines}
if [ $nLines -ne 547404 ]; then
     echo "Number of lines in pedstal ascii file is not correct, check log file to investigate possible problem (or disk space)"
     exit
fi

grep "Pedestal shift found for cell" pedestal.log > ${summaryFile}
nTotal=`wc -l ${summaryFile} | cut -f 1 -d " "`
echo " ===> Number of cells*gains for which pedestal changed " ${nTotal}
echo "      Check file "  ${summaryFile} " for the full list "
echo "      You can also look at pedestalshift_data.root for ntuple information "

echo "Produce sqlite file for LAr cell pedestalshift values" 
CaloPedestal_fillDB.py ${run1} ${lb1} ${run2} ${lb2} > makedb.log 2>&1
if [ $? -ne 0 ];  then
    echo "An error is reported, check makedb.log"
    exit
fi

if [ -f dumpDB.py ]
  then
    /bin/rm dumpDB.py
fi

cat > dumpDB.py << _EOF_
from CoolConvUtilities import AtlCoolTool
toolNew = AtlCoolTool.AtlCoolTool("sqlite://;schema=caloSqlite.db;dbname=COMP200")
toolNew.usetag("CALOOflPedestalCellPedestal-UPD4-00")
a=toolNew.more('/CALO/Ofl/Pedestal/CellPedestal')
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
echo "  After checking that everything is OK, upload the new sqlite file with:"
echo "  ~atlcond/utils/AtlCoolMerge.py caloSqlite.db  COMP200 ATLAS_COOLWRITE ATLAS_COOLOFL_CALO_W <password>"
exit
