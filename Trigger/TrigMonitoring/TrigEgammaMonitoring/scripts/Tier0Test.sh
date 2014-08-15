#!/bin/bash

####################################
#
#   Automated tests for Tier0 inclusion
#
#   - compiles package and runs AMI=q12(0|1|2) tests
#   - writes logfiles and greps for WARNING, ERROR, and all output related to HLTEgamma
#   - log files summarised
#   - also performs checks on requirements file
#
#   -> add other useful tests as you learn of them
####################################

#config
packagePath="Trigger/TrigMonitoring/TrigEgammaMonitoring"

#functions for counting number of lines in file/input and filtering files for error/warnings/etc
function countLines {
  if [ -f $1 ] ; then  
    wc $1 | awk '{print $1}'
  else
    wc < $1 | awk '{print $1}'
  fi
}
function filterWarnings {
  grep -E 'Warning|WARNING' < $1
}
function filterErrors {
  grep ERROR < $1
}
function filterPkgMsg {
  grep HLTEgamma < $1
}

#check we are in the right location (same directory the package was checked out to)
if [ ! -d $packagePath ] ; then
  echo "ERROR: Couldn't find package in working directory. Please run this script from the place you checked the package out to."
  return 1
fi

#make sure athena is set up
which athena.py >& /dev/null
if [ "$?" -ne "0" ] ; then
  echo "ERROR: No ATHENA release set up."
  return 1
fi

#remember working directory, create directory for logs and set up log summary file
wd=$( pwd )
if [ -d logs ] ; then
  datestring=$( date +%y%m%d%H%M )
  mv logs logs.$datestring
  echo "Logs directory already exists. Moving it to logs.$datestring"
fi
mkdir logs
summary="$wd/logs/summary.log"
touch $summary

#delete existing test directories
/bin/ls -d test_* 2> /dev/null | while read testDir ; do
  echo -n "Fresh test directory preferred for $testDir. May I delete the current one? (y/n) "
  read response < /dev/tty
  if [[ $response == "y" ]] ; then
    rm -rf $testDir
  fi
done

#check requirements file
cd $packagePath/cmt
echo "Checking requirements file"
echo "=== CHECKREQ ===" >> $summary
checkreq.py -a >& $wd/logs/checkreq.log
echo "exit code:        $?" >> $summary
echo -n "length of output: " >> $summary
countLines $wd/logs/checkreq.log >> $summary
cd $wd

#compile
cd $packagePath/cmt
echo "Compiling package"
echo "=== COMPILATION ===" >> $summary
(cmt config ; source setup.sh ; gmake clean ; cmt broadcast gmake ; cmt config ; source setup.sh) >& $wd/logs/compilation.log
echo "exit code: $?" >> $summary
echo -n "warnings:  " >> $summary
filterWarnings $wd/logs/compilation.log | countLines >> $summary
cd $wd

#run AMI tests
for testConfig in q120 q121 q122 g125 ; do
  echo "Testing AMI=$testConfig"
  echo "=== RECOTRF AMI=$testConfig ===" >> $summary

  #prepare folder
  mkdir "test_$testConfig"
  cd "test_$testConfig"

  #run test
  Reco_trf.py AMI=$testConfig >& ../logs/test_$testConfig.log
  echo "exit code:    $?" >> $summary
 
  #extract errors/warnings/messages and count for summary 
  cd ../logs
  filterWarnings test_$testConfig.log > test_$testConfig.warnings
  echo -n "warnings:     " >> $summary
  countLines test_$testConfig.warnings >> $summary
  filterErrors test_$testConfig.log > test_$testConfig.errors
  echo -n "errors:       " >> $summary
  countLines test_$testConfig.errors >> $summary
  filterPkgMsg test_$testConfig.log > test_$testConfig.pkgmsg
  echo -n "our messages: " >> $summary
  countLines test_$testConfig.pkgmsg >> $summary

  #return home
  cd $wd  

done
#done running AMI tests

#finish output
echo -e "All done. Dumping summary to screen now.\n"
cat $summary
