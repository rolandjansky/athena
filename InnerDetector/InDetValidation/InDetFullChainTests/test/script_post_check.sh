echo "## Running 2 script_post_check.sh "
echo "##"
echo "##"
echo "##"
echo "##"
echo "## Cleaning symlinks"
/bin/rm -f geomDB
/bin/rm -f sqlite200
/bin/rm -f triggerDB
echo "##"
echo "##"
echo "##"
echo "## Copying log file"
echo "##"
echo "##"
echo "##"
cp ${ATN_JOB_LOGFILE} InDetFullChainTests.log
echo in `pwd`
#
echo "##"
echo "##"
echo "##"
echo "## Comparing"
echo "##"
echo "##"
echo "##"
source ${ATN_PACKAGE}/test/CompareFiles.sh InDetFullChainTests.log
#
