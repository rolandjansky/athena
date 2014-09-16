#!/bin/sh
#
# Compare output of tests starting from XML and DB trigger configuration
#
# mimick timer.log created by trigtest.pl to simply show the date
starttimer=`date +%s`

# leave record of current directory in test results and create working directory
echo "List of current directory"
ls -l
echo "Moving to  AthenaXMLvsDBcompare"
#mkdir  AthenaXMLvsDBcompare
#cd  AthenaXMLvsDBcompare
pwd
date

# search for the right output: important to sort lines
LogFileXML=../AthenaXMLConfigRDO/testAthenaXMLConfigRDO_test.log
if [ -e $LogFileXML ]; then
    menuXML=`grep 'INFO Menu name:' $LogFileXML | awk '{print $NF}'`
    echo XML menu: $menuXML
    echo "Checking ${LogFileXML} for CTPSimulation and TrigChainMoniValidation"
    rm -rf REGTEST.XML.txt
    grep "CTPSimulation"  ${LogFileXML} | grep "REGTEST - |" | sort > REGTEST.XML.txt
    grep "TrigChainMoniValidation               INFO  REGTEST" ${LogFileXML} | sort >> REGTEST.XML.txt
else 
    echo "${LogFileXML} not found "
    printf "ATHENA_BAD_EXIT (8)\nATHENA_NO_LOG (16)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 120\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi

LogFileDB=../AthenaDBConfigRDO/testAthenaDBConfigRDO_test.log
if [ -e ${LogFileDB} ]; then
    menuDB=`grep 'INFO Menu name:' $LogFileDB | awk '{print $NF}'`
    echo DB menu: $menuDB
    echo "Checking ${LogFileDB} for CTPSimulation and TrigChainMoniValidation"
    rm -rf REGTEST.DB.txt
    grep "CTPSimulation" ${LogFileDB} | grep "REGTEST - |" | sort > REGTEST.DB.txt
    grep "TrigChainMoniValidation               INFO  REGTEST" ${LogFileDB} | sort >>  REGTEST.DB.txt
else 
    echo "${LogFileDB} not found"
    printf "ATHENA_BAD_EXIT (8)\nATHENA_NO_LOG (16)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 120\n maskedcode = 8\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi
echo "Good start: logfiles have been found."

# check same menus used
if [ "$menuDB" != "$menuXML" ]; then
    echo "ERROR Menu mismatch: DB $menuDB != XML $menuXML; please fix the test!"
    printf "ATHENA_BAD_EXIT (8)\nATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 120\n maskedcode = 40\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log
    exit 0
fi

# test if REGTEST.XML/DB.log files are empty and search for differences in log
rm -rf checklog.log
if [ -s REGTEST.DB.txt -a -s REGTEST.XML.txt ]; then
    diff REGTEST.DB.txt REGTEST.XML.txt > checklog.log
else
    echo "At least one of the jobs (XML or DB reading) has not been completed! Exit."
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
echo "Found $nerrors configuration differences between DB and XML jobs."

# return error code if differences found
if [ $nerrors != 0 ]; then
    echo "$nerrors differences found in XML and DB trigger configuration tests"
    printf "ATHENA_ERROR_IN_LOG (32)\nATHENA_REGTEST_FAILED (64)\n exitcode = 96\n maskedcode = 32\n" > summary.log
    cat checklog.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
else
    # otherwise return exit code 0
    echo "No differences found in XML and DB trigger configuration tests"
    printf " exitcode = 0\n maskedcode = 0\n" > summary.log
    (( timer=`date +%s` - $starttimer )); printf "$timer $starttimer `date +%s`" > timer.log 
    exit 0
fi

