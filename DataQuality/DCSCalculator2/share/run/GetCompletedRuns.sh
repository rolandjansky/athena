#!/usr/bin/env bash

cd /afs/cern.ch/user/a/atlasdqm/ws/DCSCalc/RunDCSCalc

# Takes as input the highest run number which was used in the last round.
# Don't run over this one, but do run over all others 
# (if they are recorded here then the run should have finished).

runsSince=`cat lastRunNumber`
echo "lastRunNumber = $runsSince"

python GetCompletedRuns.py $runsSince > runLog

output=`cat runLog`
echo "GetCompletedRuns.py output = $output"

perl -pi -e "s/\[//g" runLog
perl -pi -e "s/\]//g" runLog
perl -pi -e "s/,\s/\n/g" runLog

output2=`cat runLog`
echo "Tidied up output = $output2"

# Put the list of run numbers obtained into reverse order so that the first run number in the list is the latest.
sort -ru runLog > reverseOrdered
output3=`cat reverseOrdered`
echo "Puting into reverse order.  New output = $output3"
rm -f runLog
echo "Removed runLog"

# Remove the old runList
rm -f runList
echo "Removed the old runList"

# Save the latest run number for the next round to keep track of where we are.
awk NR==1 reverseOrdered > newLastRunNumber
output6=`cat newLastRunNumber`
echo "New lastRunNumber = $output6"

# Check the size of the last run number to make sure it's not empty
# (size should be == 7 if correct format).
SIZE1=`du -b newLastRunNumber | awk -F ' ' '{print $1}'`
echo "Size of newLastRunNumber = $SIZE1"
if [ $SIZE1 -le 6 ]
then
    echo "newLastRunNumber had size = $SIZE1"
    echo "newLastRunNumber  = $newLastRunNumber"
    echo "runsSince = $runsSince"

else
    # Now that we know that newLastRunNumber exists, use this to replace lastRunNumber
    echo "A new lastRunNumber exists so removing the old one."
    rm -f lastRunNumber
    cp newLastRunNumber lastRunNumber
    output4=`cat lastRunNumber`
    echo "New lastRunNumber = $output4"

    # Now put back in chronological order
    echo "Now put back in chronological order"
    sort reverseOrdered > ordered
    #rm -f runList
    
    # Remove the oldest run on the list because this should have been run beforehand.
    lastOne=`cat lastRunNumber`
    echo "lastOne = $lastOne"
    echo "runsSince = $runsSince"
    if [ $lastOne = $runsSince ]
    then 
	echo "No new runs have finished since you last checked"
    else
	echo "Checking to make sure that the previous lastRunNumber was returned by GetCompletedRuns.py"
	grep $runsSince ordered > check
        output5=`cat check`
 	echo "Result of grep $runsSince ordered (check) = $output5"  
 
	SIZE2=`du -b check | awk -F ' ' '{print $1}'`
	echo "Size of check = $SIZE2"

	if [ $SIZE2 -le 6 ]
	then
	    echo "The old lastRunNumber (runsSince = $runsSince) was not returned by GetCompletedRuns.py."
	    echo "Including all returned runs in runList."
	    cp ordered runList
	else
	    echo "The old lastRunNumber (runsSince = $runsSince) was returned by GetCompletedRuns.py."
	    echo "Excluding this run from runList."
	    sed 1d ordered > runList
	fi
	allRuns=`cat runList`
	echo "$allRuns"
	cat runList > logRunNumbers/runsToRun_upTo$lastOne
	rm -f check	
    fi
    rm -f ordered
fi

echo "Removing reverseOrdered and newLastRunNumber."
rm -f reverseOrdered
rm -f newLastRunNumber
echo "Done."
#exit
