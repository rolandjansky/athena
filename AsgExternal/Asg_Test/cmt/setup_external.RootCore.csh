Asg_Test_Dir=`dirname $1`
setenv ROOTCORE_RELEASE_SERIES `cat $Asg_Test_Dir/ReleaseSeries.txt`
setenv ASG_TEST_FILE_DATA `cat $Asg_Test_Dir/TestFileData.txt`
setenv ASG_TEST_FILE_MC `cat $Asg_Test_Dir/TestFileMC.txt`
setenv ASG_TEST_FILE_MC_AFII `cat $Asg_Test_Dir/TestFileMCAFII.txt`
setenv ROOTCORE_TEST_FILE `cat $Asg_Test_Dir/TestFileMC.txt`
setenv ROOTCORE_TEST_DATA "`dirname $ROOTCORE_TEST_FILE | xargs dirname`"
