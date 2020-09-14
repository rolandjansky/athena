#!/bin/bash
# art-description: art job for cosmic
# art-type: grid
# art-output: pid
# art-output: HLTEF-plots
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc14_cos.108865.CosSimPixVolSolOnTorOn.recon.RDO.s2111_r6258_tid05313290_00
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-output: output-dataset
# art-output: output-cost
# art-output: output-logs
# art-input-nfiles: 
# art-ncores: 8



RED='\033[0;31m'
NC='\033[0m'

function timestamp { printf "${RED}Timestamp: $1 $(date)${NC}\n"  ; }


# stuff to control running concurrent 
# processes 

PROCESS=0
BGPROCS=
NJOBS=8
WAITING=60

# is this needed ? or not ? 
# export ATHENA_NUM_PROC=$NJOBS
# export ATHENA_NUM_PROC=1

unset  ATHENA_NUM_PROC
 
env | grep ATHENA_NUM_PROC


# count how many jobs are running ...

function running {
     local IRUNNING=0
     for BGPROC in $BGPROCS; do ( ps -p $BGPROC &> /dev/null ) && ((IRUNNING++)) ; done
     echo $IRUNNING
}


# wait for an empty slot ...

function waitonproc   {
    echo "waitonproc waiting ..."
    local IRUNNING=$(running)
    while [ $IRUNNING -ge $NJOBS ]; do sleep $WAITING ; IRUNNING=$(running) ; done
}


# wait for all processes to finish ...

function waitonallproc   {
    echo "waitonallproc waiting ..."
    local IRUNNING=$(running)
    while [ $IRUNNING -gt 0 ]; do sleep $WAITING ; IRUNNING=$(running) ; done
}



# run athena  

iathena=0

function runathena { 
   timestamp  "runathena:"

   if [ $# -gt 0 ]; then  

     echo "input $1"

     pwd 

     local DIR=$PWD

     mkdir -p athena-$1
     cd  athena-$1
     cp ../*.py .

     pwd
     echo "ARGS: $ARGS"
     echo -e "\nrunning athena in athena-$1\n"
     athena.py  -c "$ARGS"               TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py  &> athena-local-$1.log
     echo "art-result: $? athena_$iathena"

     ((iathena++))

     pwd
     ls -lt

     cd $DIR

   fi  

   timestamp  "runathena done:"
}



# function to save the output from the separate 
# athena runs

function saveoutput {
    [ $# -lt 2 ] && echo "not enough arguments for saveoutput" && exit -1  
    mkdir -p $1
    for git in athena-*/$2 ; do
        FILE=$(echo  $git | sed 's|/|-|g')
        cp $git $1/$FILE
    done
}








export RTTJOBNAME=TrigInDetValidation_cosmic

jobList=

if [ $# -gt 0 -a "x$1" == "x--local" ]; then
       jobList="XMLDataSet='$RTTJOBNAME'"
else
      fileList="['${ArtInFile//,/', '}']"
      _jobList="'../${ArtInFile//,/' '../}'"
      echo "List of files = $fileList"
      for git in $_jobList ; do jobList="$jobList ARTConfig=[$git]" ; done
fi

get_files -jo             TrigInDetValidation/TrigInDetValidation_RTT_topOptions_CosmicSlice.py


# run athena in separate directories


# loop over the inputs spawning the separate athena jobs

i=0

for git in $jobList ; do 

    ARGS="$git;EventMax=;"
 
    echo "ARGS: $ARGS"

    waitonproc
    
    timestamp "forking athena index: $i"

    runathena $i & 
    PID=$!
    BGPROCS="$BGPROCS $PID"

    ((i++))

done


# wait for all the jobs to finish

sleep 10 


PPROCS=$BGPROCS

for PID in $BGPROCS ; do
     for CHILD in $(ps --ppid $PID -o pid h) ; do PPROCS="$PPROCS $CHILD" ; done 
done


ps -aF --pid $PPROCS 
ps -aF --pid $PPROCS >> topp.log

echo >> topp.log

top -b -n1 > top.log
grep PID top.log >> topp.log
for PID in $PPROCS ; do grep $PID top.log >> topp.log ; done

cat topp.log

# cat topp.log | mail sutt@cern.ch


echo -e "\n\nwaiting on athena jobs\n"

waitonallproc

echo "all done ! hooray !"

timestamp "forking all athena jobs completed"


# jobs have all finished, 
# copy the output from the athena run subdirectories to 
# single output data set directories etc

saveoutput output-dataset "Trk*.root"
saveoutput output-logs    "ath*.log"
saveoutput output-cost     trig_cost.root

timestamp "combining expert histograms ..."

# add the expert timing histgrams
hadd expert-monitoring.root athena-*/expert-monitoring.root

# this is a hack to get the script to work without needing to fix 
# TIDArun-art.sh which expects TrkNtple-0000.root
# so if we run multiple jobs then we don;t get a Trk*.root 
# file to the check will fail. This creates a link so this 
# test will pass
  
for git in output-dataset/*.root ; do ln -s $git TrkNtuple-0000.root ; break ; done  

ls -lt


get_files -data TIDAcosmic-rtt.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat

for DATFILE in *.dat ; do
    if ( grep -q DataFile $DATFILE ); then
         mv  $DATFILE  $DATFILE.bak
         grep -v "\(DataSets\|DataFiles\)"  $DATFILE.bak > $DATFILE
         echo "DataSets = { \"./output-dataset/\" };"   >> $DATFILE
    fi
done


TIDArdict TIDAcosmic-rtt.dat -f data-cosmic.root -b Test_bin.dat  2>&1 | tee TIDArdict_1.log
echo "art-result: $? TIDArdict_1"



timestamp "TIDArdict"



TIDArun-art.sh data-cosmic.root data-cosmic-reference.root HLT_id_cosmic_InDetTrigTrackingxAODCnvIOTRT_CosmicsN_EFID -d HLTEF-plots  2>&1 | tee TIDArun_2.log
echo "art-result: $? TIDArun_2"



timestamp "TIDArun-art.sh"



TIDArun-art.sh expert-monitoring.root expert-monitoring*-ref.root --auto -o times  2>&1 | tee TIDArun_3.log
echo "art-result: $? TIDArun_3"



timestamp "TIDArun-art.sh"



TIDArun-art.sh expert-monitoring.root expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF  2>&1 | tee TIDArun_4.log
echo "art-result: $? TIDArun_4"



timestamp "TIDArun-art.sh"



RunTrigCostD3PD --files output-cost/*trig_cost.root --outputTagFromAthena --costMode --linkOutputDir  2>&1 | tee RunTrigCostD3PD_5.log
echo "art-result: $? RunTrigCostD3PD_5"



timestamp "RunTrigCostD3PD"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"  2>&1 | tee TIDAcpucost_6.log
echo "art-result: $? TIDAcpucost_6"



timestamp "TIDAcpucost"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"  2>&1 | tee TIDAcpucost_7.log
echo "art-result: $? TIDAcpucost_7"



timestamp "TIDAcpucost"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"  2>&1 | tee TIDAcpucost_8.log
echo "art-result: $? TIDAcpucost_8"



timestamp "TIDAcpucost"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"  2>&1 | tee TIDAcpucost_9.log
echo "art-result: $? TIDAcpucost_9"




timestamp "TIDAcpucost"


