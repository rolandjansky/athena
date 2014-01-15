#!/bin/bash
runDIR=/afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC/Trigger/TrigCost/TrigCostAthena

cd /afs/cern.ch/user/t/trigcomm/w0/rates/schae/testarea/GenTopTriggerMC

#source setup_pro.sh

cd $runDIR/share

#xrdcp root://castoratlas/$5/$1 /tmp/.
#xrdcp root://castoratlas/$6/$2 /tmp/.
rfmkdir $4
#/afs/cern.ch/project/eos/installation/0.1.0-22d/bin/eos.select mkdir -p $4

#python $runDIR/share/mergeCostFiles.py /tmp/$1 /tmp/$2 /tmp/$3
python $runDIR/share/mergeCostFiles.py --inputFileL2 $1 --inputFileEF $2 --outputFile /tmp/$3 $7

#xrdcp /tmp/$3 root://castoratlas/$4/$3
xrdcp /tmp/$3 root://eosatlas/$4/$3
