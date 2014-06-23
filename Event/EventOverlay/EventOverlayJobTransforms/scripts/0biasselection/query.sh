#!/bin/bash

#check the bit that L1_ZB is on
#cool_read_lvl1rates.py --item L1_ZB --run $run --printSummary --printNumbers --enabledItems

#use GRL instead, see http://atlasdqm.web.cern.ch/atlasdqm/grlgen/StandardGRL/Atlas_Ready_v1/
#wget -nc http://atlasdqm.web.cern.ch/atlasdqm/grlgen/StandardGRL/Atlas_Ready_v1/data11_7TeV.periodAllYear_DetStatus-v35-pro09-03_CoolRunQuery-00-04-00_Atlas_Ready.xml
wget -nc http://atlasdqm.web.cern.ch/atlasdqm/grlgen/StandardGRL/Atlas_Ready_v1/data12_8TeV.periodAllYear_DetStatus-v49-pro13-03_CoolRunQuery-00-04-08_Atlas_Ready.xml

#See https://atlas-datasummary.cern.ch/lumicalc/, https://atlas-lumicalc.cern.ch/
#iLumiCalc.exe -t L1_ZB -V -x data/MyLBCollection.xml --lumitag=OflLumi-7TeV-000 --livetrigger=L1_EM14 >lbn
iLumiCalc.exe -t L1_ZB -V -x data12_8TeV.periodAllYear_DetStatus-v49-pro13-03_CoolRunQuery-00-04-08_Atlas_Ready.xml --lumitag=OflLumi-8TeV-002 --livetrigger=L1_EM30 > lbn
grep Total lbn

#get info on number of events per lumi block
rm lbnevents*
for runn in `grep "TGoodRunsListReader : Metadata value:" lbn | grep -v "find run "|head -1|sed -e "s%--- <DEBUG> TGoodRunsListReader : Metadata value: %%g" | sed 's/[\x01-\x1F\x7F]//g' | sed "s/\[34m//g" | sed "s/\[0m//g" | sed "s/,/ /g" ` ; do
 echo "Getting event info for run $runn "
 python GetNEventsLB.py -r $runn -s physics_ZeroBiasOverlay |grep "... Run" > lbnevents_${runn}.txt
done

#make lbn map
root -l -b -q run_lbn_analyze.C > log_lbn_analyze.txt 

echo -n "Total events in dataset: "
grep "stream 1," lbn_anal_map.txt |cut -d ' ' -f 8 |awk '{total = total + $1}END{print total}'
echo -n "Selected events per stream: "
grep "stream 8," lbn_anal_map.txt |cut -d ' ' -f 17 |awk '{total = total + $1}END{print total}'

