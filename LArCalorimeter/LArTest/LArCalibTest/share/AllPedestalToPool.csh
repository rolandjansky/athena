#!/bin/tcsh

#set tag="TB04-Default"
set tag="TB04-RndTrig-2"
set PoolFileName="LArRawConditions_H8_Pedestal_$tag.pool.root"

if ($#argv == 1) then 
 set filename=$1
 echo "Read IOV ranges from file ${filename}"
 shift
else
 echo "No filename given, run LisIOVList.exe..."
 ./LisIOVList.exe "atlobk02.cern.ch:conditions_ctb_2004:conditions:conditions" "/lar/LArElecCalibTB04/LArPedestal" PedestalIOVs_${tag}.txt ${tag} > /dev/null
  sort -n PedestalIOVs_${tag}.txt >! PedestalIOVs_${tag}_sorted.txt
  set filename=PedestalIOVs_${tag}_sorted.txt
endif

if (! -f $filename) then
 echo "File $filename not found!"
 exit
endif




echo "Sourcing basic setup" 
source ../../cmtdir/setup.csh -tag=groupArea
echo "Sourcing run-time setup"
source ${AtlasArea}/AtlasOfflineRunTime/cmt/setup.csh
echo "done with setups."

if (-f PoolFileName) then
   echo "Deleting existing POOl file ${PoolFileName}"
   rm  PoolFileName
endif


foreach line (`cat $filename`)

  set fromRun=`echo ${line} | cut -f1 -d"," `
  set fromEvent=`echo ${line} | cut -f2 -d","`
  set toRun=`echo ${line} | cut -f3 -d","`
  set toEvent=`echo ${line} | cut -f4 -d","`

  if ($fromRun<1000) then
    echo "IOV start $fromEvent to small. Ignored."
    continue
  endif
  echo "\n\nProcessing IOV from ${fromRun}/${fromEvent} to ${toRun}/${toEvent}"
  set jobOptFile="ConvertPedestal_${fromRun}_${fromEvent}_${toRun}_${toEvent}.py"
  set logFile="ConvertPedestal_${fromRun}_${fromEvent}_${toRun}_${toEvent}.log"
  echo "jobOption filename: $jobOptFile"
  echo "Logfile name: $logFile"

  if (-f $PoolFileName) then
    set DoUpdate="True"
  else
    set DoUpdate="False"
  endif

  if (-f $jobOptFile) then 
    echo "Deleting existing jobOpts $jobOptFile"
    rm $jobOptFile
  endif

  if (-f $logFile) then 
    echo "Deleting existing log-file $logFile"
    rm $logFile
  endif

  cat > ${jobOptFile} <<EOF
FromRun=${fromRun}
ToRun=${toRun}
FromEvent=${fromEvent}
ToEvent=${toEvent}
LArTB04FolderTag_Pedestal="${tag}"
OutputPOOLFileName="${PoolFileName}"
doUpdate=$DoUpdate
DoWriteCOOLIOV=True
ObjectList  = ["LArPedestalComplete#LArPedestal#/LAR/LArElecCalibH8/LArPedestal"]
TagList=["LArPedestal-"+"${tag}"]
include("LArConvertCTBConditonsToPool.py")
EOF

echo "\njobOptions:"
cat $jobOptFile


echo Starting athena.....
athena $jobOptFile > $logFile
echo ..done

end
