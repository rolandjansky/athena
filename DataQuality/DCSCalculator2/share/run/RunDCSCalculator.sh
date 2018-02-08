#!/usr/bin/env bash  

# Systems using the new calculator:
# MDT, Tile
LOG_PATH=/afs/cern.ch/user/a/atlasdqm/DQCalculators/DCSCalc/RunDCSCalc/logfiles

if [ -e "/afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/runList" ]
then

    echo "Getting COOL authentications."
    export CORAL_AUTH_PATH=$HOME/private
    
    echo "Going to run directory"
    #cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/i686-slc4-gcc34-opt
    
    #Get list of runs to loop over by sorting temporary run list and removing any duplicates.
    #list=`cat /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/runList | tr '\n' ' '`
    list=`cat /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc/runList`
    
    for run in $list
    do
	    echo "-----------------------------------------------------> NEW RUN!"
        cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/DCSCalculator/i686-slc4-gcc34-opt
        echo "Run number = $run"
	    echo "Running.........."
    
        # Original DCSCalculator
        #/afs/cern.ch/user/a/atlasdqm/DQCalculators/DCSCalc/RunDCSCalc/ExecuteDCSC1.sh $run
        
        # New DCSCalculator2
        /afs/cern.ch/user/a/atlasdqm/DQCalculators/DCSCalc/RunDCSCalc/ExecuteDCSC2.sh $run &> $LOG_PATH/dcsc2_$run
   
    	# Append the recently processed run numbers to the end of a file which keeps track of the last 50 runs which were processed.
    	cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/logfiles
    	echo "$run"
    	echo "$run" >> processedRuns_DCSOFL.log
    	sort -u processedRuns_DCSOFL.log > finishedRuns
    	rm -f processedRuns_DCSOFL.log
    	lines=`wc -l finishedRuns | awk '{print $1}'`
    	if [ $lines -gt 50 ]
    	then
        	start=`expr $lines - 50`
        	sed "1,$start d" finishedRuns > processedRuns_DCSOFL.log
    	else
        	cp finishedRuns processedRuns_DCSOFL.log
    	fi
    	rm -f finishedRuns
	
    done

    # Append the recently processed run numbers to the end of a file which keeps track of the last 50 runs which were processed.
    #cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/logfiles
    #echo "$list"
    #echo "$list" >> processedRuns_DCSOFL.log
    #sort -u processedRuns_DCSOFL.log > finishedRuns
    #rm -f processedRuns_DCSOFL.log
    #lines=`wc -l finishedRuns | awk '{print $1}'`
    #if [ $lines -gt 50 ] 
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

#exit

