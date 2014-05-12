#!/bin/zsh
#
# extract L1 L2 and EF counts from TriggerDecisionTool
#
if [ $# = 0 ]
then
   echo "Syntax: extractTD.sh  <log file>"
   exit 1
fi
echo "getting Level 1" 
grep REGTEST $1 | grep "accepted events=" | grep L1_ | sed 's/=/= /' |  awk '{ print $5, $8}' >"L1.txt"
echo "geting Level 2"
grep REGTEST $1 | grep "accepted events=" | grep L2_ | awk '{ print $5, $8}' >"L2.txt"
echo "getting EF"
grep REGTEST $1 | grep "accepted events=" | grep "EF_" | awk '{ print $5, $8}' >"EF.txt"
echo "getting total events"
grep "INFO EventCounter:EventCounter::finalize - total events:" trigdec.log >totalEvents.txt


