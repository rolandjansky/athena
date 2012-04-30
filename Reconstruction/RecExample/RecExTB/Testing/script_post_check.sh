#
 echo "Starting script_post_check.sh"
#
echo in `pwd`
#
#Cleaning
/bin/rm -f result_check
echo "*Running script_post_check.sh " > result_check
#
echo "## Comparing"
source ../Testing/CompareFiles.sh RecExTBTesting.log >> result_check
#
diff result_check ../Testing/Reference_result_check 
