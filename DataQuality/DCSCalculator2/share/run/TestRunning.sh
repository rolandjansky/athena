#!/bin/zsh 

if [ -e "/afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/runList" ]
then
   #Set up the environment and get COOL authorisations	
   echo "Setting up Athena 15.5.2.5 Tier0 release."
   source ~/cmthome/setup.sh -tag=32,opt,15.5.2.5,gcc34,oneTest,AtlasTier0

   echo "Getting COOL authentications."
   export CORAL_AUTH_PATH=$HOME/private

   echo "Going to run directory"
   cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/i686-slc4-gcc34-opt

   #Get list of runs to loop over by sorting temporary run list and removing any duplicates.
   list=`cat /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/runList`
   for run in $list
   do

   echo "-----------------------------------------------------> NEW RUN!"
   echo "Run number = $run"
   echo "Running.........."

   done

else
   echo "No new runs to process"

fi

echo "Finished"

exit

