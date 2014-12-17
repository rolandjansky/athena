#!/bin/bash

nout=20
for (( c=0; c<$nout; c+=1 )); do command="$command outputbsfilterfile${c}=%OUT.Zerobias.bit240.stream${c}.RAW" ; done

#dest="--destSE CERN-PROD_SCRATCHDISK"
#dest="--destSE BNL-OSG2_SCRATCHDISK"

#grl="data12_8TeV.periodAllYear_DetStatus-v49-pro13-03_CoolRunQuery-00-04-08_Atlas_Ready.xml"
#grl="data12_8TeV.periodAllYear_DetStatus-v61-pro14-02_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml"

#lib="--libDS user.haastyle.1110025940.3439.lib._000159"
#eout="--extOutFile=BS_multipleSelector.log";

stream="11"
iter="122"
test="test3"

#make sure we have the right map files
cp ~/nfs3/zerobias_skim/lbn_anal_map_${stream}wanted_${iter}.txt .
cp ~/nfs3/zerobias_skim/filelist_${stream}wanted_${iter}.txt .
if [ -e lbn_anal_map_${stream}wanted_${iter}.txt ]; then echo "Using lbn_anal_map_${stream}wanted_${iter}.txt";
else echo "No lbn_anal_map_${stream}wanted_${iter}.txt found"; exit; fi; 

rm -f lbn_anal_map_temp.txt
for s in $(seq 0 1 `expr $nout - 1`); do sed "s%stream 0,%stream $s,%" lbn_anal_map_${stream}wanted_${iter}.txt >> lbn_anal_map_temp.txt; done
mv lbn_anal_map_temp.txt lbn_anal_map_${stream}wanted_${iter}.txt

#data="data12_8TeV.*.physics_ZeroBias.*.RAW"
data="data12_8TeV.*.physics_ZeroBiasOverlay.*.RAW"
#split="--nGBPerJob=10"
split="--fileList=`cat filelist_${stream}wanted_${iter}.txt`"

echo "Command is: pathena --trf \"BS_multipleSelector_trf.py inputbsfile=%IN $command Noutputs=$nout lbnmapfile=lbn_anal_map_${stream}wanted_${iter}.txt TriggerBit=240 \" --extFile=lbn_anal_map_${stream}wanted_${iter}.txt,filelist_${stream}wanted_${iter}.txt $eout --outDS user.ahaas.BSmultipleselector_select${stream}.$test --fileList=\`cat filelist_${stream}wanted_${iter}.txt\` --inDS $data $lib $dest --noBuild "
pathena --trf "BS_multipleSelector_trf.py inputbsfile=%IN $command Noutputs=$nout lbnmapfile=lbn_anal_map_${stream}wanted_${iter}.txt TriggerBit=240 " --extFile=lbn_anal_map_${stream}wanted_${iter}.txt,filelist_${stream}wanted_${iter}.txt $eout --outDS user.ahaas.BSmultipleselector_select${stream}.$test $split --inDS $data $lib $dest --noBuild 
#--nFiles 50 --express
#--site ANALY_FZK
#--excludedSite ANALY_IN2P3-CC,ANALY_BNL_ATLAS
#--v
#--individualOutDS

#rm lbn_anal_map_${stream}wanted_${iter}.txt filelist_${stream}wanted_${iter}.txt

