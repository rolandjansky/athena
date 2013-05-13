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
cat InDetMonitoringATN.log
echo "##"
echo "##"
echo "##"
echo "## Create and dump perfmon summary"
echo "##"
echo "##"
echo "##"
perfmon.py ntuple.pmon.gz
cat ntuple.perfmon.summary.txt
