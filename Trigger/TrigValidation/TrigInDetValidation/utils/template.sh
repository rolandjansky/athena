
RED='\033[0;31m'
NC='\033[0m'

# generate a time stamp

function timestamp { printf "${RED}Timestamp: $1 $(date)${NC}\n"  ; }

# poor quality function to convert seconds to a proper time

function converttime { 
    local totes=$1
    [ $totes -lt 0 ] && ((totes=$totes+86400))
    ((H=$totes/3600))
    ((M=($totes%3600)/60))
    ((S=$totes%60))
    echo "$H : $M : $S"
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
     athena.py  -c "$ARGS"   @REPLACEJOBOPTIONS  &> athena-local-$1.log
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








export RTTJOBNAME=@REPLACERTTJOBNAME

jobList=

if [ $# -gt 0 -a "x$1" == "x--local" ]; then
       jobList="XMLDataSet='$RTTJOBNAME'"
else
      fileList="['${ArtInFile//,/', '}']"
      _jobList="'../${ArtInFile//,/' '../}'"
      echo "List of files = $fileList"
      for git in $_jobList ; do jobList="$jobList ARTConfig=[$git]" ; done
fi

get_files -jo @REPLACEJOBOPTIONS


# run athena in separate directories


# loop over the inputs spawning the separate athena jobs

i=0

for git in $jobList ; do 

    ARGS="$git;@REPLACECOMMAND"
 
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

[ -e topp.log ] && rm topp.log

ps -aF --pid $PPROCS | grep $USER >> topp.log

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
  
for git in output-dataset/*.root ; do ln -s $git TrkNtuple-0000.root ; break ; done  

ls -lt


