#
 echo "Starting script_post_check.sh"
#
 cd $BFIELDATHTESTROOT/run
#
#Cleaning
 /bin/rm -f result_check
 echo "*Running script_post_check.sh " > result_check
#
#Comparing
 source ../Testing/CompareFiles.sh Out.Test01 Out.Test02 Out.Test03 >> result_check
#
#Testing
 diff result_check ../Testing/Reference_result_check 
