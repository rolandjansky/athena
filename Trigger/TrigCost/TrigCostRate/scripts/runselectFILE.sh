#!/bin/bash
###--------------------------------------------------------
### Run job
#source runselectFILE.sh /afs/cern.ch/user/s/schae/testarea/AtlasWork str(currRUNNR) str(item) str(stream) stream_temp
#bsub -q atlastrig source /afs/cern.ch/user/t/trigrate/testarea/rel16/Trigger/TrigCost/TrigCostRate/scripts/runselectFILE.sh /afs/cern.ch/user/t/trigrate/testarea/rel16 175360 L2HltTimeout 0
#print_debug 0156663-EFCostMonitoring EFHltError EFMissingData for 117 0161562-EFCostMonitoring
###--------------------------------------------------------
### print castor debuging information
print_debug()
{
  echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
  # switch on debugging and switch to different disk
  export RFIO_TRACE=3     # zsh / bash syntax
  export STAGER_TRACE=3   # zsh / bash syntax
  export STAGE_SVCCLASS=atlasgroupdisk

  # get details about the settings
  hostname
  date
  uname -a
  id
  env | grep RFIO
  env | grep STAGE
  env | grep PATH
  rpm -qa| grep castor
  which rfcp
  castor -v
  echo "@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@"
}

###--------------------------------------------------------
### Check input options
if [ "$#" -ne "4" ]
then
    echo "Incorrect number of parameters: $#"; exit 1
fi

###--------------------------------------------------------
### Setup ATLAS release and define local variables
SETDIR=$1
SCRIPT=/afs/cern.ch/user/t/trigrate/cmthome/setup.sh
WRITEDIR=`pwd`
#OUTDIR=/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer
#OUTDIR=/afs/cern.ch/atlas/project/trigger/trigcaf/schae
OUTDIR=/afs/cern.ch/user/t/trigcomm/scratch0
echo "the run dir $1"
cd $SETDIR #-tag=AtlasProduction,15.6.8.6,opt,slc4,runtime,32

echo `pwd`
pushd `pwd`
source setup_pro.sh

cd $SETDIR/Trigger/TrigCost/TrigCostRate/scripts
source $SETDIR/Trigger/TrigCost/TrigCostRate/cmt/setup.sh
source $SETDIR/Trigger/TrigCost/TrigCostBase/cmt/setup.sh
popd
echo "the testarea: $SETDIR"
echo "Removing any old log files, finished files, and waitfiles"
rm $OUTDIR/logfiles/log_$2_$3.log
rm $OUTDIR/donefiles/filesthataredone_$2_$3.dat
rm $OUTDIR/waitfiles/waitforfiles_$2_$3.dat

#new writing area
mkdir -p $WRITEDIR/0$2-$3

#runcommand="python /afs/cern.ch/user/s/schae/testarea/AtlasWork/PhysicsNtuple/TrigMonCosts/scripts/selectFILE.py --currRun="+str(currRUNNR)+" --data --do-time --runnr="+str(item)+" --stream="+str(stream)+" -o '$OUTDIR/0"+str(item)+"-"+stream_temp+"'"+" &> log_"+str(item)+"_"+str(stream)+" &"
#    run_="$SETDIR/PhysicsNtuple/TrigMonCosts/macros/runRates.py $2 -o $3 -n $4 --data --do-time -k COSTMON"
#make root file name
    #run_="python $SETDIR/PhysicsNtuple/TrigMonCosts/scripts/selectFILE.py --reSubmit=$4 --logDir=$1 --runnr=$2 --stream=$3 -o $OUTDIR/0$2-$3 --data --do-time -k COSTMON -r $OUTDIR/0$2-$3/cost_0$2_$3.root &> logfiles/log_$2_$3 &"

    #baserun_="python $SETDIR/Trigger/TrigCost/TrigCostRate/scripts/selectFILE.py --reSubmit=$4 --logDir=$1 --runnr=$2 --stream=$3 -o $OUTDIR/0$2-$3 --data --do-time -k COSTMON --XMLoutputfile=/afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml --rosXML=/afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml -r $OUTDIR/0$2-$3/cost_0$2_$3.root --wrt-lb 30 "

    baserun_="python $SETDIR/Trigger/TrigCost/TrigCostRate/scripts/selectFILE.py --reSubmit=$4 --runnr=$2 --stream=$3 -o $WRITEDIR/0$2-$3 --data --do-time -r $WRITEDIR/0$2-$3/cost_0$2_$3.root --wrt-lb 2000 -n 1100000 --evt-lb 50000 "

    if [[ "$3" != *CostMonitoring* ]] 
    then
	echo "This is a Debug Stream"
	baserun_="$baserun_ --debug-stream --XMLoutputfile=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml -k TrigCost  "
    fi

    if [[ "$3" == *CostMonitoring* ]] 
    then
	echo "This is a CostMonitoring Stream"
	baserun_="$baserun_ -k SFO-4 " #-k COSTMON "
    fi

    if [[ "$4" == 0 ]]
    then
	echo "Collecting the ROS-ROB map!"
	baserun_="$baserun_ --XMLoutputfile=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml "
    fi

    if [[ "$4" == *-1* ]] 
    then
	echo "Do NOT collect ROS-ROB maps"
	baserun_="$baserun_ --stopMap --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml "
    fi

    if [[ "$4" == *-2* ]]
    then
        echo "Do NOT collect ROS-ROB maps"
        baserun_="$baserun_ --stopMap --debug --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-0$2-$3.xml "
    fi

   if [[ "$4" == *2012* ]]
    then
        echo "Do NOT collect ROS-ROB maps for 2011"
        baserun_="$baserun_ --stopMap --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-robin-2012.xml "
    fi

   if [[ "$4" == *2011* ]]
    then
        echo "Do NOT collect ROS-ROB maps for 2011"
        baserun_="$baserun_ --stopMap --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-2011.xml "
    fi

    if [[ "$4" == *2010* ]]
    then
        echo "Do NOT collect ROS-ROB maps for 2010"
        baserun_="$baserun_ --stopMap --debug --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-2010.xml "
    fi


    run_="$baserun_ &> logfiles/log_$2_$3 &"
    #run_="$baserun_"

echo "Starting job at "`date`
echo "Working directory: "`pwd`
echo "Executing command: $run_"
echo "-------------------------------------------------------------------------------------------------------"

$run_

cd $WRITEDIR
ls $WRITEDIR

tar -czf 0$2-$3.tar.gz 0$2-$3/

FILESIZE=$(stat -c%s 0$2-$3.tar.gz)
echo "Size of 0$2-$3.tar.gz = $FILESIZE bytes."

if [[ $FILESIZE -gt 40087 ]]
then
    cp -r $WRITEDIR/0$2-$3.tar.gz $OUTDIR/t0cost_archives/.
    echo "Checking that tar file has been copied correctly:"
    ls $OUTDIR/t0cost_archives/0$2-$3.tar.gz
    
    cksum $OUTDIR/t0cost_archives/0$2-$3.tar.gz 0$2-$3.tar.gz

else
    echo "Did not copy File because jobs did not finish: $OUTDIR/0$2-$3.tar.gz"
fi

echo "-------------------------------------------------------------------------------------------------------"
echo "Finished job at "`date`