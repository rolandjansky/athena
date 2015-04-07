#!/bin/bash

#use GRL
wget -nc http://atlasdqm.web.cern.ch/atlasdqm/grlgen/All_Good/data12_8TeV.periodAllYear_DetStatus-v61-pro14-02_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml

#See https://atlas-datasummary.cern.ch/lumicalc/, https://atlas-lumicalc.cern.ch/
#iLumiCalc.exe -t L1_ZB -V -x data12_8TeV.periodAllYear_DetStatus-v61-pro14-02_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml --lumitag=OflLumi-8TeV-003 --livetrigger=L1_EM30 --lar --lartag=LARBadChannelsOflEventVeto-UPD4-04 > lbn
iLumiCalc.exe -t L1_ZB -V -x data12_8TeV.periodAllYear_DetStatus-v61-pro14-02_DQDefects-00-01-00_PHYS_StandardGRL_All_Good.xml --lumitag=OflLumi-8TeV-003 --livetrigger=L1_EM30 > lbn
grep Total lbn

#get info on number of events and files per lumi block in each run
rm -f lbnevents*
rm -f runs_lbn_files.txt
sstream="physics_ZeroBiasOverlay"
#sstream="physics_ZeroBias"
for runn in `echo "200842,200863,200913,200926,200965,200967,200982,200987,201006,201052,201113,201120,201138,201190,201191,201257,201269,201280,201289,201489,201494,201555,201556,202660,202668,202712,202740,202798,202965,202991,203027,203169,203191,203195,203228,203256,203258,203277,203335,203336,203353,203432,203454,203456,203523,203524,203602,203605,203636,203680,203719,203739,203745,203760,203779,203792,203875,203876,203934,204025,204026,204071,204073,204134,204153,204158,204240,204265,204416,204442,204474,204564,204633,204668,204726,204763,204769,204772,204796,204853,204857,204910,204932,204954,204955,204976,205010,205016,205017,205055,205071,205112,205113,206368,206369,206409,206497,206573,206614,206955,206962,206971,207044,207046,207221,207262,207304,207306,207332,207397,207447,207490,207528,207531,207532,207582,207589,207620,207664,207696,207749,207772,207800,207809,207845,207864,207865,207931,207934,207975,207982,208123,208126,208179,208184,208189,208258,208261,208354,208485,208631,208642,208662,208705,208717,208720,208780,208781,208811,208870,208930,208931,208970,208982,209024,209025,209074,209084,209109,209161,209183,209214,209254,209265,209269,209353,209381,209550,209580,209608,209628,209629,209736,209776,209787,209812,209864,209866,209899,209980,209995,210302,210308,211620,211670,211697,211772,211787,211867,211902,211937,212034,212103,212142,212144,212172,212199,212272,212619,212663,212687,212721,212742,212809,212815,212858,212967,212993,213039,213079,213092,213130,213155,213157,213204,213250,213359,213431,213479,213486,213539,213627,213640,213684,213695,213702,213754,213796,213816,213819,213900,213951,213964,213968,214021,214086,214160,214176,214216,214388,214390,214494,214523,214544,214553,214618,214651,214680,214714,214721,214758,214777,215027,215061,215063,215091,215414,215433,215456,215464,215473,215541,215571,215589,215643"|sed "s%,% %g"` ; do
 echo "Getting event info for run $runn "
 python GetNEventsLB.py -r $runn -s $sstream |grep "... Run" > lbnevents_${runn}.txt
 dq2-ls -f -H data12_8TeV.00${runn}.physics_ZeroBiasOverlay.merge.RAW >> runs_lbn_files.txt
done
grep "_lb" runs_lbn_files.txt > runs_lbn_files_lb.txt

#make lbn_anal_map.txt
root -l -b -q run_lbn_analyze.C > ~/nfs3/zerobias_skim/log_lbn_analyze.txt 

echo -n "Total events in dataset before GRL: "
cat lbnevents_*.txt |cut -d ' ' -f 8 |awk '{total = total + $1}END{print total}'
echo -n "Total events in dataset from log_lbn_analyze.txt: "
grep "stream 0" ~/nfs3/zerobias_skim/log_lbn_analyze.txt|cut -d ' ' -f 6 |awk '{total = total + $1}END{print total}'
echo -n "Total events in dataset from lbn_anal_map: "
grep "stream 1," lbn_anal_map.txt |cut -d ' ' -f 8 |awk '{total = total + $1}END{print total}'
echo -n "Selected events per stream: "
grep "stream 8," lbn_anal_map.txt |cut -d ' ' -f 17 |awk '{total = total + $1}END{print total}'

#do this for all 50 streams
for s in {0..49}; do grep "stream ${s}," lbn_anal_map.txt | grep -v "0 want" > ~/nfs3/zerobias_skim/lbn_anal_map_${s}wanted.txt ; done
rm lbn_anal_map.txt

#split into jobs for each stream
python lbn_anal_map_splitter.py 
rm ~/nfs3/zerobias_skim/*501.txt

for s in {0..49}; do
 echo "renaming stream $s to 0"
 for f in {1..500}; do sed -i -e "s%stream $s,%stream 0,%g" ~/nfs3/zerobias_skim/lbn_anal_map_${s}wanted_${f}.txt; done
done

cd ~/nfs3/zerobias_skim/
it=2
for s in {0..49}; do
 tar cfz ${s}wanted${it}.tar.gz *_${s}wanted_*.txt
done
cd -

#send to afs
scp ~/nfs3/zerobias_skim/*wanted${it}.tar.gz ahaas@lxplus.cern.ch:public/overlay/lists/

#to check the events in each map:
for s in {0..49}; do
 echo -n "files with 100 events in stream $s :"
 for f in {1..500}; do cat ~/nfs3/zerobias_skim/lbn_anal_map_${s}wanted_$f.txt | cut -d ' ' -f 17 |awk '{total = total + $1}END{print total}'; done |grep -c 100
done

#to show number of files in each filelist:
for f in ~/nfs3/zerobias_skim/filelist_0wanted_*.txt; do echo -n `grep -o "data12" $f | wc -l`; echo " : $f"; done |sort -n

