#!/bin/sh
# ERROR_MESSAGE :circular
# TEST_SUITE    :circular dependence
 
echo " ======================================================== "
echo " Starting test with cmt show cycles "
echo " ======================================================== "
cd ${NIGHTLYAREA}/AtlasRelease/*/cmt
outputx=`cmt show cycles`
stat=$?
output=`echo $outputx | grep -v "slug atdummy"` 
if [ "$stat" != "0" ]; then
        echo " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "
        echo " FAILURE : test cmt show cycles returned $stat "
        echo " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "
fi
if [ "$output" != "" ]; then
        echo " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "
        echo " PROBLEM : test cmt show cycles found circular dependencies"
        echo " $output "
        echo " !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! "
fi
 
