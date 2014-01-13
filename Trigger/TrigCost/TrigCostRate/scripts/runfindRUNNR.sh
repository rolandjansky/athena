#!/bin/bash
#command for cron job
#>acrontab -e
#*/30 * * * * lxplus source /afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/scripts/runfindRUNNR.sh atlastrig /afs/cern.ch/user/t/trigrate/testarea/AtlasCost &> /afs/cern.ch/user/t/trigrate/testarea/AtlasCost/Trigger/TrigCost/TrigCostRate/scripts/logFile_Cron.log
#
#### Check input options
if [ "$#" -ne "2" ]
then
   echo "Incorrect number of parameters: $#"; exit 1
fi

####--------------------------------------------------------
### Setup ATLAS release and define local variables
SETDIR=$2
#SETUPSCRIPT=/afs/cern.ch/user/t/trigrate/cmthome/setup.sh
RUNDIR=$SETDIR/Trigger/TrigCost/TrigCostRate/scripts

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
cd $SETDIR
echo $SETDIR
echo "source the setup script: $SETDIR/setup_pro.sh"
source $SETDIR/setup_pro.sh
cd $RUNDIR
echo "the current directory: $RUNDIR"

#make root file name
if [ $1 -eq "1" ]
then
    run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/scripts/findRUNNR.py --testarea=$SETDIR"
else
    if [ $1 -eq "8" ]
    then
	run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/scripts/findRUNNR.py -q 8nh --testarea=$SETDIR"
    else
	run_="python $SETDIR/Trigger/TrigCost/TrigCostRate/scripts/findRUNNR.py" # -q $1 --testarea=$SETDIR
    fi
fi
echo "Starting job at "`date`
echo "Working directory: "`pwd`
echo "Executing command: $run_"
echo "-------------------------------------------------------------------------------------------------------"

$run_

echo "-------------------------------------------------------------------------------------------------------"
echo "Finished job at "`date`