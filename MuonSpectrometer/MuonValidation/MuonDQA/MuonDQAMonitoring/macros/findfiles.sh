#!/bin/bash

# $1 is runnumber, i.e. 0090733

echo $path
echo " " > streamsfiles.txt
exec<streams.txt
echo "Searching for files..."

while read line
do
        path="/castor/cern.ch/grid/atlas/tzero/prod1/perm/data08_cosmag/physics_$line/$1"
        folder=`nsls $path | grep HIST`
	if [ $folder ] ; then
	    file=`nsls $path/$folder/ | grep HIST`
	    if [ $file ]; then
		echo $line
		echo "$line $path/$folder/$file" >> streamsfiles.txt
	    else
		echo "$line --------->> NO HIST FILE FOUND!!!"
	    fi
	else
	    echo "$line ---->> NOT FOUND FOR THIS RUN!!!"
	fi
done
