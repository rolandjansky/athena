#
 echo "Starting script_pre_check.sh"
#
 cd $BFIELDATHTESTROOT/run
#
#Cleaning
 /bin/rm -f Out.Test01 
 /bin/rm -f Out.Test02 
 /bin/rm -f Out.Test03 
 /bin/rm -f BFieldAthTesting.log
#
#Set All
 source $BFIELDATHTESTROOT/share/BFieldAthTestSetAll.sh
 cd $BFIELDATHTESTROOT/run
#
