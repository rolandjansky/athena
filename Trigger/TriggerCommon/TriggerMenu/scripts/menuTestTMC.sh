#!/bin/env zsh

if [[ $# -ne 1 ]]; then
    echo "Usage: " `basename $0` "LVL1config.xml"
    exit 1
fi

testdir=test_`date +%F_%T`
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
echo "Output log: ${testdir}/log"

echo runTriggerMenuCompilerApp.sh --dir ./ -u -m $xmlfile -c ./ctp_ddr.xml -s smxi.dat -l lut.dat -k cam.dat -x mon_sel_ -y mon_dec_ -w smx_ -t smxo.dat -p 1 > tmc.cmd

runTriggerMenuCompilerApp.sh --dir ./ -u -m $xmlfile -c ./ctp_ddr.xml -s smxi.dat -l lut.dat -k cam.dat -x mon_sel_ -y mon_dec_ -w smx_ -t smxo.dat -p 1 >! log

grep "ERROR" log >! errorlog

if [ -s errorlog ]; then
   print "==>> ERROR found when running TriggerMenuCompiler on $xmlfile:\n"
   cat errorlog
else
   print "TriggerMenuCompiler ran successfully on $xmlfile"
fi

cp errorlog ../
