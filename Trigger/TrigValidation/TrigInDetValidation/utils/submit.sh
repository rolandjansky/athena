#!/bin/bash

echo "version 2"

echo -e "\nChecking for new data sets to be added to the art container ... \n"

DUFF=$(mktemp -u /tmp/.submitDS-XXXX.sh)

!( which rucio >& /dev/null ) && echo "rucio not found" && exit -1

touch $DUFF

rucio list-content user.artprod.input.container | grep -v SCOPE:NAME |  grep -v + | awk '{print $2}' > $DUFF

rm -rf newDS.txt .newDS.txt

DATASETS=$(mktemp -u /tmp/.DS-XXXX.txt)
touch $DATASETS

for FILE in ../test/test*.sh ; do 

  SKIP=1

  DS=$(grep "art-input:" $FILE | sed 's|.*art-input:[[:space:]]*||g') 

  NF=$(grep "^# *art-input-nfiles:" $FILE | sed 's|.*art-input-nfiles: *||g') 

#  printf "\n----------------------------\n$FILE  \tDS: $DS  \tNF: $NF\n"

  # echo "DS: $DS" 
  if ! ( grep -q $DS $DUFF ); then 

        if [ -e newDS.txt ]; then 
	    ( grep -q " $DS " newDS.txt ) || printf "\n new dataset: $DS\n"
	fi

        [ ! -e .$DS ] && rucio list-files $DS &> .$DS
   
	( grep -q "Total files" .$DS ) && SKIP=0

	grep -q "Total files" .$DS

	if [ $SKIP -eq 0 ]; then 

            NFILES=$(grep "Total files" .$DS | sed 's|.*:[[:space:]]||g')

	    if [ $NFILES -gt 0 ]; then 	

		[ -e newDS.txt ] || echo -e "\nPlease add the following datasets to user.artprod.input.container\n" > newDS.txt

		if ( grep -q " $DS " newDS.txt ); then 
		    
		    NOF=$(cat newDS.txt | grep " $DS " | sed 's|.*(||g' | sed 's|files.*||g') 
		    
		    if [ $NOF -lt $NF ]; then 

			for NDS in {,.}newDS.txt ; do mv $NDS{,-} ; grep -v " $DS " ${NDS}- > $NDS ; rm ${NDS}- ; done
			
			echo " replacing:  $DS   ( $NF files )"
		       	printf " * $DS   \t( $NF files )\n" >>  newDS.txt
			printf   " $DS   \t( $NF files )\n" >> .newDS.txt 
		    fi
		else
		    echo $DS >> $DATASETS
		    echo " adding:     $DS   ( $NF files )"
      		    printf " * $DS   \t( $NF files )\n" >>  newDS.txt
		    printf   " $DS   \t( $NF files )\n" >> .newDS.txt 
		fi
            fi

	else 
	    if [ $SKIP -eq 1 ]; then 
                echo "dataset for $FILE does not exists on the grid - disabling test"
	        mv $FILE $(echo $FILE | sed 's|test/test_|test/|g')
	    fi
	fi

#        rm .$DS
  fi


done


if [ -e newDS.txt ]; then 
    printf "\nnew datasets to be added:\n\n"
   cat .newDS.txt
   /usr/bin/cern-get-sso-cookie --url https://its.cern.ch/jira/ -o cookiefile.txt  284  11:30 /usr/bin/cern-get-sso-cookie --url https://its.cern.ch/jira/ -o cookiefile.txt
   ../utils/submit.py -f newDS.txt
   rm newDS.txt .newDS.txt
   rm $DUFF
   rm $DATASETS
else
   printf "\nno datasets to be added\n"
fi



