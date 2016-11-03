Asg_Test_Dir=`dirname $1`
export ROOTCORE_RELEASE_SERIES=`cat $Asg_Test_Dir/ReleaseSeries.txt`
export ASG_TEST_FILE_DATA=`cat $Asg_Test_Dir/TestFileData.txt`
export ASG_TEST_FILE_MC=`cat $Asg_Test_Dir/TestFileMC.txt`
export ASG_TEST_FILE_MC_AFII=`cat $Asg_Test_Dir/TestFileMCAFII.txt`
export ROOTCORE_TEST_FILE=`cat $Asg_Test_Dir/TestFileMC.txt`
export ROOTCORE_TEST_DATA="`dirname $ROOTCORE_TEST_FILE | xargs dirname`"
