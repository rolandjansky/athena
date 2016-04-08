#
 echo "Starting script_pre_check.sh"
#
 cd $IPATRECEXAMPLEROOT/run
#
#Cleaning
echo "## cleaning"
 /bin/rm -f *stripped
 /bin/rm -f iPatRecExampleTesting.log
#echo "## setting up run time files"

#for G3
 source ../share/DC1_Setup.sh

#for G4
# source ../share/DC2_Setup.sh

#
#Set All
 cd $IPATRECEXAMPLEROOT/run
#
echo "## setting up environment "
 source ../cmt/setup.sh
echo "## ready to run"
