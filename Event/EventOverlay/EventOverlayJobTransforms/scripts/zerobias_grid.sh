#!/bin/bash
for (( c=0; c<50; c+=1 )); do command="$command outputbsfilterfile${c}=%OUT.Zerobias.bit240.stream${c}.RAW" ; done

dest="--destSE CERN-PROD_SCRATCHDISK"
#dest="--destSE BNL-OSG2_SCRATCHDISK"

#grl="data12_8TeV.periodAllYear_DetStatus-v49-pro13-03_CoolRunQuery-00-04-08_Atlas_Ready.xml"
grl="data12_8TeV.periodAllYear_DetStatus-v61-pro14-02_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml"
#lib="--libDS user.haastyle.1110025940.3439.lib._000159"

test="test12a1"
#eout="--extOutFile=BS_multipleSelector.log"; #for test12p5

data="data12_8TeV.*.physics_ZeroBias.*.RAW"
#data="data12_8TeV.*.physics_ZeroBiasOverlay.*.RAW"
split="--nGBPerJob=10"

#data="data12_8TeV.00202660.physics_ZeroBiasOverlay.merge.RAW,data12_8TeV.00203456.physics_ZeroBiasOverlay.merge.RAW"
#split="--nFilesPerJob=5"

#make sure we have the right map file, and back it up
mv -i -v lbn_anal_map.txt lbn_anal_map_${test}.txt ; #call the new one by the test name
#mv -v lbn_anal_map_*.txt ~/nfs3/lbn_anal_map/ ; #move them all to backup
#cp -u -v ~/nfs3/lbn_anal_map/lbn_anal_map_${test}.txt . ; #bring back the one we want
if [ -e lbn_anal_map_${test}.txt ]; then echo "Using lbn_anal_map_${test}.txt";
else echo "No lbn_anal_map_${test}.txt found!"; exit; fi; ls -l lbn_anal_map* ;

pathena --trf "BS_multipleSelector_trf.py inputbsfile=%IN $command lbnmapfile=lbn_anal_map_${test}.txt TriggerBit=240 " --extFile=lbn_anal_map_${test}.txt $eout --outDS group.dataprep.BSmultipleselector.GRL.${test} --official "--voms=atlas:/atlas/dataprep/Role=production" --individualOutDS $split --memory=3000 --goodRunListXML $grl --goodRunListDataType RAW --goodRunListDS $data $lib $dest 
#--nFiles 50 --express
#--site ANALY_FZK
#--excludedSite ANALY_IN2P3-CC,ANALY_BNL_ATLAS
#--v

