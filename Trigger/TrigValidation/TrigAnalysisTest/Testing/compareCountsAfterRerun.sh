#!/bin/sh
#
# Compare output of tests starting from a new AOD and an AOD produced by
# re-running the hypos on the same new AOD: all counts should be the same
# 
# mimick timer.log created by trigtest.pl to simply show the date
starttimer=`date +%s`

# leave record of current directory in test results and create working directory
echo "List of current directory"
ls -l
echo "Moving to compareCountsAfterRerun"
mkdir AthenaTrigAOD_rerun_compareCounts
cd AthenaTrigAOD_rerun_compareCounts
pwd
date

# search for the right output: important to sort lines
if [ -e ../AthenaTrigAOD_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log ]; then
    echo "grepping ../AthenaTrigAOD_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log "
    rm -rf REGTEST.FIRST.txt
    grep "TrigDecChecker " ../AthenaTrigAOD_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log | grep "REGTEST item " | sort > REGTEST.FIRST.txt
    grep "TrigDecChecker " ../AthenaTrigAOD_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log | grep "REGTEST chain " | sort >> REGTEST.FIRST.txt
else 
    echo "../AthenaTrigAOD_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log not found "
    printf "ATHENA_BAD_EXIT (8)\nATHENA_NO_LOG (16)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 120\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
if [ -e ../AthenaTrigAOD_rerun_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log ]; then
    echo "grepping testAthenaTrigAOD_rerunHypo_TrigDecTool_test.log "
    rm -rf REGTEST.RERUN.txt
    grep "TrigDecChecker " ../AthenaTrigAOD_rerun_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log | grep "REGTEST item " | sort > REGTEST.RERUN.txt
    grep "TrigDecChecker " ../AthenaTrigAOD_rerun_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log | grep "REGTEST chain " | sort >> REGTEST.RERUN.txt
else 
    echo "../AthenaTrigAOD_rerun_TrigDecTool/testAthenaTrigAOD_TrigDecTool_test.log not found "
    printf "ATHENA_BAD_EXIT (8)\nATHENA_NO_LOG (16)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 120\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
echo "Logfiles have been found!"

# test if REGTEST.FIRST/RERUN.log files exist and are not empty and search for differences in log
if [ -s REGTEST.FIRST.txt -a -s REGTEST.RERUN.txt ]; then
    diff REGTEST.FIRST.txt REGTEST.RERUN.txt > checklog.log
else
    echo "At least one of the jobs (from initial AOD or from AOD after rerunning hypos on initial AOD) has not been completed! Exit."
    echo "At least one of the jobs (from initial AOD or from AOD after rerunning hypos on initial AOD) has not been completed! Exit." >> checklog.log
    ls
    printf "POST_COMMAND1_BAD_EXIT (2)\nATHENA_BAD_EXIT (8)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 106\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
echo "Logfile diff successful"

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
echo "Found $nerrors differences in trigger counts between initial RDO->AOD and AOD->rerun->AOD"

# return error code if differences found
if [ $nerrors != 0 ]; then
    echo "$nerrors differences in trigger counts between initial RDO->AOD and AOD->rerun->AOD"
    printf "ATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 96\n maskedcode = 32\n" > summary.log
    cat checklog.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
else
    # otherwise return exit code 0
    echo "No differences found in trigger counts between initial RDO->AOD and AOD->rerun->AOD"
    printf " exitcode = 0\n maskedcode = 0\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi

