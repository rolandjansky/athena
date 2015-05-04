#!/bin/env zsh

if [[ $# -ne 1 ]]; then
    echo "Usage: " `basename $0` "LVL1config.xml"
    exit 1
fi

testdir=testTMC_`date +%F_%T`
xmlfile=$1 

rm -rf $testdir
mkdir $testdir

cp $xmlfile $testdir


cd $testdir

get_files -xmls -symlink LVL1config.dtd > /dev/null
get_files -xmls -symlink ctp_ddr.xml > /dev/null
get_files -xmls -symlink CTP_DDR.dtd > /dev/null


echo "Running TriggerMenuCompiler on $xmlfile"
echo "Output directory: $testdir"
echo "Output log file: ${testdir}/log"

echo runTriggerMenuCompilerApp.sh --dir ./ -u -m $xmlfile -c ./ctp_ddr.xml -s smxi.dat -l lut.dat -k cam.dat -x mon_sel_ -y mon_dec_ -w smx_ -t smxo.dat -p 1 > tmc.cmd

{
   runTriggerMenuCompilerApp.sh --dir ./ -u -m $xmlfile -c ./ctp_ddr.xml -s smxi.dat -l lut.dat -k cam.dat -x mon_sel_ -y mon_dec_ -w smx_ -t smxo.dat -p 1 >! log 2>&1
   #runTriggerMenuCompilerApp.sh --dir ./ -u -m $xmlfile -c ./ctp_ddr.xml -s smxi.dat -l lut.dat -k cam.dat -x mon_sel_ -y mon_dec_ -w smx_ -t smxo.dat -p 1 | tee log 2>&1
 
   grep -A 1 "ERROR" log >! errorlog
   
   if [ -s errorlog ]; then
      print "==>> ERROR found when running TriggerMenuCompiler on $xmlfile:\n"
      cat errorlog
   else
      print "SUCCESS : TriggerMenuCompiler ran successfully on $xmlfile"
   fi
 
   cp errorlog ../
   print "Output log file: $testdir/log"
} &

print "Running TMC in the background, wait until it is finished. If it takes more than 20 seconds, then you should kill the job and contact the CTP (Ralf and Joerg)"
