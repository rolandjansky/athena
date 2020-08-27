#!/bin/bash
# art-description: art job for el_Zee_IBL_pu40
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-html: https://idtrigger-val.web.cern.ch/idtrigger-val/TIDAWeb/TIDAart/?jobdir=
# art-type: grid
# art-output: HLTEF-plots
# art-output: HLTL2-plots
# art-output: HLTEF-plots-lowpt
# art-output: HLTL2-plots-lowpt
# art-output: times
# art-output: times-FTF
# art-output: cost-perCall
# art-output: cost-perEvent
# art-output: cost-perCall-chain
# art-output: cost-perEvent-chain
# art-input:  mc15_13TeV.361106.PowhegPythia8EvtGen_AZNLOCTEQ6L1_Zee.recon.RDO.e3601_s2665_s2183_r7191
# art-output: *.dat 
# art-output: *.root
# art-output: *.log
# art-output: output-dataset
# art-output: output-cost
# art-output: output-logs
# art-input-nfiles: 8
# art-cores: 8
# art-memory: 4096



RED='\033[0;31m'
NC='\033[0m'


function usage { 
    [ $# -gt 1 ] && echo $2

    echo "Usage: $(basename $0) [args]"
    echo 
    echo "-d, --directory  DIRECTORY \t run from the specified directory"
    echo "-l, --local                \t run locally rather than on the grid"
    echo "-x, --exclude              \t don't run athena or the post processing, only the plotting stages"
    echo "-p, --post                 \t force running of post processingthe post processing, even if -x is set"
    echo "-f, --force                \t disable protection against rerunning where you shouldn't be"
    echo "-h, --help                 \t this help"
    [ $# -gt 0 ] && exit $1
    exit 0
}

args=$(getopt -ql "searchpath:" -o "d:lxph" -- "$@")

# eval set -- "$args"

RUNATHENA=1
RUNPOST=-1
DIRECTORY=
LOCAL=0
FORCE=0
FORK=1

while [ $# -ge 1 ]; do
    case "$1" in
        --) shift ; break ;;
        -d | --directory )  if [ $# -lt 2 ]; then usage; fi ; DIRECTORY="$2" ; shift ;;
        -x | --exclude )    RUNATHENA=0 ; [ $RUNPOST -eq -1 ] && RUNPOST=0;;
        -p | --post )       RUNPOST=1 ;;
        -f | --force )      FORCE=1 ;;
        -l | --local )      LOCAL=1 ;;
        -h | --help )       usage ;;
     esac
    shift
done


[ $RUNPOST -eq 0 ] || RUNPOST=1


# generate a time stamp

function timestamp { printf "${RED}Timestamp: $1 $(date)${NC}\n"  ; }

# poor quality function to convert seconds to a proper time

function converttime { 
    local totes=$1
    [ $totes -lt 0 ] && ((totes=$totes+86400))
    ((H=$totes/3600))
    ((M=($totes%3600)/60))
    ((S=$totes%60))
    printf "%d:%02d:%02d\n" $H $M $S
}

timestamp "starting"

# reset seconds counter - not really needed, but hey 

SECONDS=0


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
unset  ATHENA_PROC_NUMBER
 
env | grep ATHENA_NUM_PROC
env | grep ATHENA_PROC_NUMBER




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

function runathena { 
   timestamp  "runathena:"

   if [ $# -gt 0 ]; then  

     echo "input $1"

     pwd 

     local DIR=$PWD

     mkdir -p athena-$1
     cd  athena-$1

     pwd
     echo "ARGS: $ARGS"
     echo -e "\nrunning athena in athena-$1\n"
     athena.py  -c "$ARGS"              TrigInDetValidation/TrigInDetValidation_RTT_topOptions_ElectronSlice.py  &> athena-local-$1.log
     echo "art-result: $? athena_$1"

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





ls -l



export RTTJOBNAME=TrigInDetValidation_el_Zee_IBL_pu40

jobList=

if [ $LOCAL -eq 1 ]; then

      echo "running locally"
      # get number of files 
      NFILES=$(grep "^#[[:space:]]*art-input-nfiles:" $0 | sed 's|.*art-input-nfiles:[[:space:]]*||g')
      [ $NFILES -lt 1 ] && echo "not enough files: $NFILES" && exit -1
      DATASET=$(grep "^#[[:space:]]*art-input:" $0 | sed 's|.*art-input:[[:space:]]*||g')
      _jobList=$(\ls /eos/atlas/atlascerngroupdisk/proj-sit/trigindet/$DATASET/* )
      for git in $_jobList ; do [ $NFILES -gt 0 ] || break ; jobList="$jobList ARTConfig=['$git']" ; ((NFILES--)) ; echo "running over $git"  ; done

elif [ -n "$ArtProcess" ]; then

      # art-cores specified, so ART is already forking off subprocesses
      FORK=0
      case "$ArtProcess" in
            start)
                  timestamp "ART Starting (${ArtCores}-core)"
                  exit 0   # nothing more to do this time round
                  ;;
            end)
                  timestamp "ART Ending (${ArtCores}-core)"
                  ;;       # skip to postprocessing
            *)
                  # runathena here and now, no forking
                  timestamp "ART job $ArtProcess (${ArtCores}-core)"
                  IFS=',' read -r -a file <<< "${ArtInFile}"
                  file=${file[${ArtProcess}]}
                  _jobList="'../$file'"
                  echo "ART running over $_jobList"
                  jobList="ARTConfig=[$_jobList]"
                  if [ $RUNATHENA -eq 1 ]; then
                      ARGS="$jobList;EventMax=2000"
                      runathena "$ArtProcess"
                  fi
                  exit 0   # this thread is done
                  ;;
      esac

else

      fileList="['${ArtInFile//,/', '}']"
      _jobList="'../${ArtInFile//,/' '../}'"
      echo "List of files = $fileList"
      for git in $_jobList ; do jobList="$jobList ARTConfig=[$git]" ; echo "ART running over $git"  ; done

fi


if [ $RUNATHENA -eq 1 ]; then 

if [ $FORK -eq 1 ]; then

# run athena in separate directories


# loop over the inputs spawning the separate athena jobs

i=0

for git in $jobList ; do 

    ARGS="$git;EventMax=2000"
 
#   echo "ARGS: $ARGS"

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

[ -e topp.log ] && rm topp.log

echo -e "\nUID        PID  PPID  C    SZ   RSS PSR STIME TTY          TIME CMD" >> topp.log
ps -aF --pid $PPROCS | grep $USER | grep -v grep | grep -v sed | sed 's| [^[:space:]]*/python | python |g' | sed 's| [^[:space:]]*/athena| athena|g' | sed 's|ARTConfig=.* |ARTConfig=... |g' | sed 's|eos/[^[:space:]]*/trigindet|eos/.../trigindet|g' >> topp.log

echo >> topp.log

sleep 20 

top -b -n1 > top.log
grep PID top.log >> topp.log
for PID in $PPROCS ; do grep $PID top.log >> topp.log ; done

cat topp.log

# cat topp.log | mail sutt@cern.ch


echo -e "\n\n"

timestamp "waiting on athena jobs ..."

# echo -e "\n\nwaiting on athena jobs...\n"

waitonallproc

fi

echo "all done ! hooray !"

timestamp "forking all athena jobs completed"


# jobs have all finished, 
# copy the output from the athena run subdirectories to 
# single output data set directories etc

saveoutput output-dataset "Trk*.root"
saveoutput output-logs    "ath*.log"
saveoutput output-cost     trig_cost.root

grep ERROR   athena-*/ath*.log | grep -v ERRORS | grep -v MATRIX | grep -v DERIV &> errors.log 
grep WARNING athena-*/ath*.log &> warnings.log

timestamp "combining expert histograms ..."

# add the expert timing histgrams
hadd expert-monitoring.root athena-*/expert-monitoring.root &> hadd.log 

# this is a hack to get the script to work without needing to fix 
# TIDArun-art.sh which expects TrkNtple-0000.root
# so if we run multiple jobs then we don;t get a Trk*.root 
# file to the check will fail. This creates a link so this 
# test will pass
  
for git in output-dataset/*.root ; do if [ -e $git ]; then ln -s $git TrkNtuple-0000.root ; break ; fi ; done  

[ -e TrkNtuple-0000.root ] || echo "WARNING: all athena stages failed"

fi


ls -lt


get_files -data TIDAdata11-rtt.dat
get_files -data TIDAdata_cuts.dat
get_files -data TIDAdata_chains.dat
get_files -data TIDAbeam.dat
get_files -data Test_bin.dat

for DATFILE in *.dat ; do
    if ( grep -q DataFile $DATFILE ); then
         mv  $DATFILE  $DATFILE.bak
         grep -v "\(DataSets\|DataFiles\)"  $DATFILE.bak > $DATFILE
         echo "DataSets = { \"./output-dataset/\" };"   >> $DATFILE
    fi
done

if [ $RUNATHENA -eq 1 -o $RUNPOST -eq 1 ]; then


TIDArdict TIDAdata11-rtt.dat -f data-electron-merge.root -p 11 -b Test_bin.dat  2>&1 | tee TIDArdict_1.log
echo "art-result: $? TIDArdict_1"



timestamp "TIDArdict"



fi


TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig -d HLTEF-plots  2>&1 | tee TIDArun_2.log
echo "art-result: $? TIDArun_2"



timestamp "TIDArun-art.sh"



TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40-reference.root HLT_e24_medium_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-plots  2>&1 | tee TIDArun_3.log
echo "art-result: $? TIDArun_3"



timestamp "TIDArun-art.sh"



TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_IDTrig -d HLTEF-plots-lowpt  2>&1 | tee TIDArun_4.log
echo "art-result: $? TIDArun_4"



timestamp "TIDArun-art.sh"



TIDArun-art.sh data-electron-merge.root data-el_Zee_IBL_pu40-reference.root HLT_e5_loose_idperf_InDetTrigTrackingxAODCnv_Electron_FTF -d HLTL2-plots-lowpt  2>&1 | tee TIDArun_5.log
echo "art-result: $? TIDArun_5"



timestamp "TIDArun-art.sh"



TIDArun-art.sh expert-monitoring.root expert-monitoring*-ref.root --auto -o times  2>&1 | tee TIDArun_6.log
echo "art-result: $? TIDArun_6"



timestamp "TIDArun-art.sh"



TIDArun-art.sh expert-monitoring.root expert-monitoring*-ref.root --auto -p FastTrack -o times-FTF  2>&1 | tee TIDArun_7.log
echo "art-result: $? TIDArun_7"



timestamp "TIDArun-art.sh"



RunTrigCostD3PD --files output-cost/*trig_cost.root --outputTagFromAthena --costMode --linkOutputDir  2>&1 | tee RunTrigCostD3PD_8.log
echo "art-result: $? RunTrigCostD3PD_8"



timestamp "RunTrigCostD3PD"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall --auto -d "/Algorithm" -p "_Time_perCall"  2>&1 | tee TIDAcpucost_9.log
echo "art-result: $? TIDAcpucost_9"



timestamp "TIDAcpucost"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent --auto -d "/Algorithm" -p "_Time_perEvent"  2>&1 | tee TIDAcpucost_10.log
echo "art-result: $? TIDAcpucost_10"



timestamp "TIDAcpucost"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perCall-chain --auto -d "/Chain_Algorithm" -p "_Time_perCall"  2>&1 | tee TIDAcpucost_11.log
echo "art-result: $? TIDAcpucost_11"



timestamp "TIDAcpucost"



TIDAcpucost costMon/TrigCostRoot_Results.root costMon/TrigCostRoot_Results.root -o cost-perEvent-chain --auto -d "/Chain_Algorithm" -p "_Time_perEvent"  2>&1 | tee TIDAcpucost_12.log
echo "art-result: $? TIDAcpucost_12"




timestamp "TIDAcpucost"



printf "${RED}done: $SECONDS seconds${NC}\n"


printf "${RED}done: job duration:  $(converttime $SECONDS)${NC}\n"

