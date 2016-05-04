#!/bin/sh

# Check that rerunning hypothesis algorithms on AOD with tuned selection yields expected trigger counts.
# In this case e10_medium selection criteria are applied to e5_medium chain. The trigger counts for both chains must then be the same.
# 
# mimick timer.log created by trigtest.pl to simply show the date
starttimer=`date +%s`
#
# leave record of current directory in test results and create working directory
echo "List of current directory"
ls -l
#echo "Moving to checkCountsAfterRerun_tunedCuts"
#mkdir AthenaTrigAOD_rerun_tunedCuts_checkCounts
#cd AthenaTrigAOD_rerun_tunedCuts_checkCounts
pwd
date

# search for the right output

if [ -e ../AthenaTrigAOD_rerun_tunedCuts_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log ]; then
    echo "grepping testAthenaTrigAOD_TrigDecTool_test.log "
    rm -rf REGTEST_e10.txt
    rm -rf REGTEST_e5.txt
    grep "TrigDecChecker " ../AthenaTrigAOD_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log | grep "REGTEST chain " | grep "L2_e10_medium " | sort > REGTEST_e10.txt
    grep "TrigDecChecker " ../AthenaTrigAOD_rerun_tunedCuts_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log | grep "REGTEST chain " | grep "L2_e5_medium " | sort > REGTEST_e5.txt

else 
    echo "../AthenaTrigAOD_rerun_tunedCuts_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log not found "
    printf "ATHENA_BAD_EXIT (8)\nATHENA_NO_LOG (16)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 120\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
echo "Logfiles have been found!"

# test if REGTEST.FIRST/RERUN.log files exist and are not empty and search for differences in log
if [ -s REGTEST_e5.txt -a -s REGTEST_e10.txt ]; then
    sed 's/e10_medium/e5_medium/g' REGTEST_e10.txt > REGTEST_e10_forDiff.txt
    diff REGTEST_e5.txt REGTEST_e10_forDiff.txt > checklog.log
else
    echo "Job from AOD with tuned cuts after rerunning hypos has not been completed! Exit."
    echo "Job from AOD with tuned cuts after rerunning hypos has not been completed! Exit." >> checklog.log
    ls
    printf "POST_COMMAND1_BAD_EXIT (2)\nATHENA_BAD_EXIT (8)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 106\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
echo "Logfile printout successful."

# count line differences and decide exit code
if [ -e checklog.log ]; then
    nerrors=0
    while read line
      do
      (( nerrors += 1 ))
    done < checklog.log
else
    echo "checklog.log not found"
    printf "POST_COMMAND1_BAD_EXIT (2)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 98\n maskedcode = 2\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi

# return error code if differences found
if [ $nerrors != 0 ]; then
    echo "Differences found in trigger counts between e5_medium and e10_medium when rerunning on AOD with e10_medium hypothesis selection on e5_medium."
    printf "ATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 96\n maskedcode = 32\n" > summary.log
    cat checklog.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
else
    # otherwise return exit code 0
    echo "No differences found in trigger counts between e5_medium and e10_medium when rerunning on AOD with e10_medium hypothesis selection on e5_medium."
    printf " exitcode = 0\n maskedcode = 0\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi

