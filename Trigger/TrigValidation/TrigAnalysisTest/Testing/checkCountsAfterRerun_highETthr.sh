#!/bin/sh
#
# Check that rerunning hypothesis algorithms on AOD with unrealistically high selection on tranverse energy yields zero trigger counts.
# In this case the e10_loose chain is used and the energy threshold for rerunning is set 100 times higher.
# 
# mimick timer.log created by trigtest.pl to simply show the date
starttimer=`date +%s`
#
# leave record of current directory in test results and create working directory
echo "List of current directory"
ls -l
#echo "Moving to checkCountsAfterRerun_highETthr"
#mkdir AthenaTrigAOD_rerun_highETthr_checkCounts
#cd AthenaTrigAOD_rerun_highETthr_checkCounts
pwd
date

# search for right output

if [ -e ../AthenaTrigAOD_rerun_highETthr_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log ]; then
    echo "grepping testAthenaTrigAOD_TrigDecTool_test.log "
    rm -rf REGTEST.e10_loose_highETthr.txt
    grep "TrigDecChecker " ../AthenaTrigAOD_rerun_highETthr_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log | grep "REGTEST chain " | grep "L2_e10_loose " | sort > REGTEST.e10_loose_highETthr.txt
else 
    echo "../AthenaTrigAOD_rerun_highETthr_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log not found "
    printf "ATHENA_BAD_EXIT (8)\nATHENA_NO_LOG (16)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 120\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
echo "Logfile has been found!"

# test if REGTEST.e10_loose_highETthr.txt file exists and is not empty and search for differences with reference REGTEST.REF.e10_loose_highETthr.txt
if [ -s REGTEST.e10_loose_highETthr.txt ]; then
    rm -rf REGTEST.REF.e10_loose_highETthr.txt
    echo "TrigDecChecker                                     INFO REGTEST chain L2_e10_loose accepted events= 0 ( PS: 0 , PT: 0)" > REGTEST.REF.e10_loose_highETthr.txt
    diff REGTEST.e10_loose_highETthr.txt REGTEST.REF.e10_loose_highETthr.txt > checklog.log
else
    echo "The rerunning of hypothesis algorithms on AOD with a high energy threshold on the e10_loose chain has not been completed! Exit." >> checklog.log
    ls
    printf "POST_COMMAND1_BAD_EXIT (2)\nATHENA_BAD_EXIT (8)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 106\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
echo "Logfile diff successful."

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
    echo "Trigger counts for e10_loose when rerunning hypothesis algorithms on AOD with 100 times higher energy threshold are not 0."
    printf "ATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 96\n maskedcode = 32\n" > summary.log
    echo "Both lines above should be identical."
    cat checklog.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
else
    # otherwise return exit code 0
    echo "Trigger counts for e10_loose when rerunning hypothesis algorithms on AOD with 100 times higher energy threshold are 0 as expected."
    printf " exitcode = 0\n maskedcode = 0\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
