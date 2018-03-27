#!/bin/bash

echo -e "\nChecking for new data sets to be added to the art container ... \n"

DUFF=$(mktemp -u /tmp/.submitDS-XXXX.sh)

!( which rucio >& /dev/null ) && echo "rucio not found" && exit -1

rucio list-content user.artprod.input.container | grep -v SCOPE:NAME |  grep -v + | awk '{print $2}' > $DUFF

rm -rf newDS.txt .newDS.txt

for FILE in ../test/test*.sh ; do 

  SKIP=1

  DS=$(grep "art-input:" $FILE | sed 's|.*art-input:[[:space:]]*||g') 

  # echo "DS: $DS" 
  if ! ( grep -q $DS $DUFF ); then 
        echo "dataset not included: $DS"
  
        rucio list-files $DS &> .$DS
   
	( grep -q "Total files" .$DS ) && SKIP=0


	if [ $SKIP -eq 0 ]; then 

            NFILES=$(grep "Total files" .$DS | sed 's|.*:[[:space:]]||g')

	    if [ $NFILES -gt 0 ]; then 	
#		echo "NFILES: $NFILES"
		[ -e newDS.txt ] || echo -e "\nPlease add the following datasets to user.artprod.input.container\n" > newDS.txt
		echo " * $DS" >>  newDS.txt  
		echo "$DS"    >> .newDS.txt  
            fi
	else 
	    if [ $SKIP -eq 1 ]; then 
                echo "dataset for $FILE does not exists on the grid - disabling test"
	        mv $FILE $(echo $FILE | sed 's|test/test_|test/|g')
	    fi
	fi

        rm .$DS
  fi


done

if [ -e newDS.txt ]; then 
   echo "new datasets to be added:"
   cat .newDS.txt
   /usr/bin/cern-get-sso-cookie --url https://its.cern.ch/jira/ -o cookiefile.txt  284  11:30 /usr/bin/cern-get-sso-cookie --url https://its.cern.ch/jira/ -o cookiefile.txt
   ../utils/submit.py -f newDS.txt
   rm newDS.txt .newDS.txt
else
   echo "no datasets to be added"
fi



