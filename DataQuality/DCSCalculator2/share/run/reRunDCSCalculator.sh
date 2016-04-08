#!/usr/bin/env bash 

if [ -e "/afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/reRunList" ]
then


    #Get list of runs to loop over by sorting temporary run list and removing any duplicates.
    list=`cat /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/reRunList`
    for run in $list
    do

        echo "-----------------------------------------------------> NEW RUN!"
        cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/i686-slc4-gcc34-opt
        echo "Run number = $run"
    
        # Original DCSCalculator
#        /afs/cern.ch/user/a/atlasdqm/DQCalculators/DCSCalc/RunDCSCalc/ExecuteDCSC1.sh $run
        
        # New DCSCalculator2
        /afs/cern.ch/user/a/atlasdqm/DQCalculators/DCSCalc/RunDCSCalc/ExecuteDCSC2.sh $run
	
    done

    # Append the recently processed run numbers to the end of a file which keeps track of the last 50 runs which were processed.
    #cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/logfiles
    #cd /afs/cern.ch/user/k/kleney/GetCompletedRuns
    #echo "$list" >> processedRuns_DCSOFL.log
    #sort -u processedRuns_DCSOFL.log > finishedRuns
    #rm -f processedRuns_DCSOFL.log
    #lines=`wc -l finishedRuns | awk '{print $1}'`
    #if [ lines -gt 50 ] 
    #then
	#start=`expr $lines - 50`
	#sed "1,$start d" finishedRuns > processedRuns_DCSOFL.log
    #else 
	#cp finishedRuns processedRuns_DCSOFL.log
    #fi
    #rm -f finishedRuns
    
else
    echo "No new runs to process"
fi

echo "Finished"

cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc

