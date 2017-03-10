#!/bin/bash

#setupATLAS
#lsetup rucio
#voms-proxy-init -voms atlas

#use GRL, from: http://atlasdqm.web.cern.ch/atlasdqm/grlgen/All_Good/?C=M;O=D
#consult: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/GoodRunListsForAnalysisRun2
#and: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DataMCForAnalysis

#dq="data15_13TeV.periodAllYear_DetStatus-v73-pro19-08_DQDefects-00-01-02_PHYS_StandardGRL_All_Good.xml"
#wget -nc http://atlasdqm.web.cern.ch/atlasdqm/grlgen/All_Good/$dq

#Can just go here: https://atlas-lumicalc.cern.ch/  ... note use --online lumi tag for HI, unless a better one is available
#and request --verbose output and use physics trigger HLT_noalg_zb_L1ZB, 
#then wget "Raw iLimuiCalc.exe output", and "mv output.txt lbn_2015; ln -s lbn_2015 lbn"
#rm -f lbn # if you want to redo this
if [ ! -f "lbn" ]; then 
iLumiCalc.exe -t L1_ZB -V -x $dq --livetrigger=L1_EM12 > lbn
fi
grep ": Total" lbn ; echo

#get info on number of events and files per lumi block in each run
#rm -f lbnevents* # if you want to redo this
rm -f runs_lbn_files.txt # if you want to redo this
sstream="physics_ZeroBias" #pp
#sstream="physics_MinBiasOverlay" #HI
rm -f runs_temp.txt
for runn in `grep "subchild node value" lbn|cut -f 3 -d : |sed 's/[\x01-\x1F\x7F]//g'|sed 's%\[0m%%g'|sed 's%\[34m%%g'|sed ':a;N;$!ba;s/\n/ /g' `; do
 if [ ! -f lbnevents_${runn}.txt ]; then 
   echo "Getting Nevent info for run $runn "; 
   python GetNEventsLB.py -r $runn -s $sstream |grep "... Run" > lbnevents_${runn}.txt;# Only works on lxplus now!
 fi 
 if [ ! -f runs_lbn_files.txt ]; then 
   echo "Getting file info for run $runn "
   rucio list-file-replicas data16_13TeV.00${runn}.${sstream}.merge.RAW | grep "CERN-PROD_DATADISK" | cut -d '|' -f 6|sed "s% CERN-PROD_DATADISK: gsiftp://eosatlassftp.cern.ch:2811/%root://eosatlas.cern.ch//%" | sed -r 's/\s+//g' | grep "_lb" >> runs_temp.txt #pp 2015
   #rucio list-file-replicas data15_hi.00${runn}.${sstream}.daq.RAW | grep "CERN-PROD_DATADISK" | cut -d '|' -f 6|sed "s% CERN-PROD_DATADISK: gsiftp://eosatlassftp.cern.ch:2811/%root://eosatlas.cern.ch//%" | sed -r 's/\s+//g' | grep "_lb" >> runs_temp.txt #HI 2015
   echo -n "Replicated? : "; rucio list-dataset-replicas data16_13TeV.00${runn}.${sstream}.merge.RAW|grep CERN-PROD_DATADISK
   nfound=`grep -c data runs_temp.txt`
   echo "Found $nfound files so far"
 fi
done #loop over all runs
if [ ! -f runs_lbn_files.txt ]; then mv runs_temp.txt runs_lbn_files.txt ; fi

rm -f lbn_anal_map.txt # if you want to redo this
if [ ! -f lbn_anal_map.txt ]; then root -l -b -q run_lbn_analyze.C > log_lbn_analyze.txt ; fi

echo -n "Total events in dataset before GRL: "
cat lbnevents_*.txt |tr -s ' ' | cut -d ' ' -f 8 |awk '{total = total + $1}END{print total}'
echo -n "Total events in dataset from log_lbn_analyze.txt: "
grep "stream 0" log_lbn_analyze.txt|cut -d ' ' -f 6 |awk '{total = total + $1}END{print total}'
echo -n "Total events in dataset from lbn_anal_map: "
grep "stream 1," lbn_anal_map.txt |cut -d ' ' -f 8 |awk '{total = total + $1}END{print total}'
echo -n "Selected events per stream: "
grep "stream 8," lbn_anal_map.txt |cut -d ' ' -f 17 |awk '{total = total + $1}END{print total}'

maxstream=0 #0 up to 49
#split into all the desired streams
for s in $(seq 0 $maxstream); do 
  grep "stream ${s}," lbn_anal_map.txt | grep -v "and 0 wanted" > lbn_anal_map_stream${s}.txt
  mkdir -p output_stream${s}
done

#split into jobs for each stream and make filelist and lbn_anal_map files
rm -f output_stream*/filelist_*.txt output_stream*/lbn_anal_map_*.txt
python lbn_anal_map_splitter.py $maxstream > log_lbn_anal_map_splitter.txt
rm output_stream*/*501.txt

grep "not in files map" log_lbn_anal_map_splitter.txt
if [ $? == 0 ]; then exit; fi

for s in $(seq 1 $maxstream); do
 echo "renaming stream $s to 0"
 for f in {1..500}; do sed -i -e "s%stream $s,%stream 0,%g" output_stream${s}/lbn_anal_map_${f}.txt; done
done

it=2016_pp_1 #just a name to tag this set of files
for s in $(seq 0 $maxstream); do cd output_stream${s}; tar cfz stream${s}_${it}.tar.gz *.txt; cd ..; done
mv -v output_stream*/stream*_${it}.tar.gz ~/public/overlay/lists/

#to check the events in each map:
for s in $(seq 0 $maxstream); do
 echo -n "files with 100 events in stream $s :"
 for f in {1..500}; do cat output_stream${s}/lbn_anal_map_$f.txt | cut -d ' ' -f 17 |awk '{total = total + $1}END{print total}'; done |grep -c 100
done

#to show number of files in each filelist:
echo ; echo "files in each filelist (in stream 0)..." ; for f in output_stream0/filelist_*.txt; do echo -n `grep -o $sstream $f | wc -l`; echo " : $f"; done |sort -n

