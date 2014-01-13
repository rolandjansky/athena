#!/bin/bash
###--------------------------------------------------------
### Run job
#source run_LB.sh /afs/cern.ch/user/s/schae/testarea/AtlasWork str(currRUNNR) str(item) str(stream) stream_temp beg-lb end-lb
#bsub -q atlastrig source /afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/scripts/run_LB.sh /afs/cern.ch/user/t/trigrate/testarea/AtlasCost 167607 L2CostMonitoring 0 94 94
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
if [ "$#" -ne "6" ]
then
    echo "Incorrect number of parameters: $#"; exit 1
fi

###--------------------------------------------------------
### Setup ATLAS release and define local variables
SETDIR=$1

echo "the run dir $1"
cd $SETDIR #-tag=AtlasProduction,15.6.8.6,opt,slc4,runtime,32

#mkdir -p $1_$3_$4
#cd $1_$3_$4
echo `pwd`
pushd `pwd`
#cd `pwd`

source setup_pro.sh
#AtlasProduction,15.6.9.18,opt,slc4,runtime,32
#AtlasP1HLT,15.6.9.18,runtime,opt,32
#-tag=AtlasP1HLT,15.6.9.20,runtime,opt,32
cd $SETDIR/Trigger/TrigCost/TrigCostRate/scripts
source $SETDIR/Trigger/TrigCost/TrigCostRate/cmt/setup.sh
popd
echo "the testarea: $SETDIR"
echo "Removing any old log files, finished files, and waitfiles"
#old writing area
#mkdir -p /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/0$2-$3
#new writing area
mkdir -p /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/0$2-$3-beglb$5-$6

#runcommand="python /afs/cern.ch/user/s/schae/testarea/AtlasWork/PhysicsNtuple/TrigMonCosts/scripts/selectFILE.py --currRun="+str(currRUNNR)+" --data --do-time --runnr="+str(item)+" --stream="+str(stream)+" -o '/afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/0"+str(item)+"-"+stream_temp+"'"+" &> log_"+str(item)+"_"+str(stream)+" &"
#    run_="$SETDIR/PhysicsNtuple/TrigMonCosts/macros/runRates.py $2 -o $3 -n $4 --data --do-time -k COSTMON"
#make root file name
    #run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/scripts/selectFILE.py --reSubmit=$4 --logDir=$1 --runnr=$2 --stream=$3 -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/0$2-$3-beglb$5-$6_ros-robv4 --data --do-time -k COSTMON --beg-lb=$5 --end-lb=$6 --rosXML=/afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/scripts/maps/rob-ros-pixmod-v4.xml --runModXML=/afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/scripts/Run_167607-pixmod-v4.xml -r /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/0$2-$3-beglb$5-$6_ros-robv4/cost_0$2_$3.root &> log_$2_$3_v4 &"
OUTROOT=/afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC/Trigger/TrigCost/TrigCostPython/macros
    #run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/scripts/run_doug_run.py --reSubmit=$4 --logDir=$1 --runnr=$2 --stream=$3 -o /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/0$2-$3-beglb$5-$6 --data -k COSTMON --beg-lb=$5 --end-lb=$6 --rosXML=/afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/data/rob-ros-2011.xml  -r /afs/cern.ch/user/t/trigcomm/w0/rates/rates_webpage/schaefer/0$2-$3-beglb$5-$6/cost_0$2_$3.root &> log_$2_$3_v4 &"

#     run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/share/runCosts.py /castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/calibration_$3/00$2/data11_7TeV.00$2.calibration_$3.Reproc.NTUP_COSTMON.c338/ -o /tmp/0$2-$3-beglb$5-$6-$4 --data -k $4 --rosXML=$SETDIR/Trigger/TrigCost/TrigCostRate/data/rob-ros-2011.xml  --beg-lb=$5 --end-lb=$6 -r out.root --test='roiplot' "

# LATEST
     run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/share/runCosts.py /castor/cern.ch/grid/atlas/caf/atlcal/perm/trigger/data11_7TeV/calibration_$3/00$2/data11_7TeV.00$2.calibration_$3.Reproc.NTUP_COSTMON.c338/ -o /tmp/0$2-$3-beglb$5-$6-$4 --beg-lb $5 --end-lb $6 --data -k $4 -r /tmp/knewestout$2-$3$4$5.root --test='roiplot' "
#FOR CAF
#run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/share/runCosts.py /eos/atlas/atlasdatadisk/data11_7TeV/NTUP_TRIGCOSTL2/r2825/data11_7TeV.00$2.physics_EnhancedBias.recon.NTUP_TRIGCOSTL2.r2825_tid543483_00 -o /tmp/0$2-$3-beglb$5-$6-$4 --rosXML=/afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/data/rob-ros-2011.xml --data -k $4 -r /tmp/out$3$5.root --test='roiplot' "



echo "Starting job at "`date`
echo "Working directory: "`pwd`
echo "Executing command: $run_"
echo "-------------------------------------------------------------------------------------------------------"

$run_
cp /tmp/knewestout$2-$3$4$5.root $OUTROOT/.
echo "-------------------------------------------------------------------------------------------------------"
echo "Finished job at "`date`