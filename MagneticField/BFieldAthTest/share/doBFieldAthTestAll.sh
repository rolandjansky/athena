#
 echo "Starting doBFieldAthTestAll.sh"
#
 cd $BFIELDATHTESTROOT/run
#
#Cleaning
 /bin/rm -f Out.Test01 
 /bin/rm -f Out.Test02 
 /bin/rm -f Out.Test03 
 /bin/rm -f athena.log
#
#Set All
 source $BFIELDATHTESTROOT/share/BFieldAthTestSetAll.sh
 cd $BFIELDATHTESTROOT/run
#
#Run
 ../$CMTCONFIG/athena.exe jobOptions.txt > athena.log
#

