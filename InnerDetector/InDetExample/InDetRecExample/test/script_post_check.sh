echo "## Running script_post_check.sh "
echo "##"
echo in `pwd`
echo "##"
echo "##"
echo "##"
echo "## Cleaning symlinks"
/bin/rm -f geomDB
/bin/rm -f sqlite200
echo "##"
echo "##"
echo "##"
echo "## Dump log file"
echo "##"
echo "##"
echo "##"
cat InDetRecExampleTesting.log
#
echo "##"
echo "##"
echo "##"
echo "## Comparing"
echo "##"
echo "##"
echo "##"
source ../test/CompareFiles.sh InDetRecExampleTesting.log
#
