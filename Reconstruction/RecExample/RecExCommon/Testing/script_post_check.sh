echo "Starting script_post_check.sh"
#
echo "Starting script_post_check.sh"
#

#
echo "## Comparing : "
echo "> < Reference log file  "
echo "> > New log file  "
source ../Testing/CompareFiles.sh RecExCommonTesting.log
if [ $? -ne 0 ] ; then
    echo "FAILURE log file differ significantly from reference log file "
else
    echo "SUCCESS : log file does not differ significantly from reference log file "
fi