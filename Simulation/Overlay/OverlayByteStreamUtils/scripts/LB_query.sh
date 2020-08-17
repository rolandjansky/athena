#!/bin/bash
#
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# NOTE: Rucio setup is required

set -e

# use GRL, from: http://atlasdqm.web.cern.ch/atlasdqm/grlgen/All_Good/?C=M;O=D
# consult: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/GoodRunListsForAnalysisRun2
# and: https://twiki.cern.ch/twiki/bin/view/AtlasProtected/DataMCForAnalysis

# TODO: read from args
#sstream="physics_ZeroBias" #pp or UPC
sstream="physics_MinBiasOverlay" #HI
pref="data18_hi"
#suf="merge"
suf="daq"
# just a name to tag this set of files
out_name=data18_hi_1

# TODO: read from args
dq="http://atlasdqm.web.cern.ch/atlasdqm/grlgen/HeavyIons/All_Good/data18_hi.periodS_DetStatus-v104-pro22-08_Unknown_PHYS_HeavyIonP_All_Good.xml"
wget -nc "$dq"

# Can just go here: https://atlas-lumicalc.cern.ch/  ... note use --online lumi tag for HI, unless a better one is available
# and request --verbose output and use physics trigger HLT_noalg_zb_L1ZB, 
# then wget "Raw iLimuiCalc output", and "mv output.txt lbn_2015; ln -s lbn_2015 lbn"

# if you want to redo this:
# rm -f lbn
if [ ! -f "lbn" ]; then 
  # iLumiCalc -t L1_ZB -V -x "$dq" --livetrigger=L1_EM12 > lbn
  iLumiCalc -t L1_ZB --lumitag OflLumi-HI-009 -V -x "$dq" --livetrigger=L1_EM12 | tee lbn
fi
grep ": Total" lbn ; echo

# get info on number of events and files per lumi block in each run
# if you want to redo this:
# rm -f lbnevents*
# rm -f runs_lbn_files.txt

rm -f runs_temp.txt
runs=$(grep "subchild node value" lbn|cut -f 3 -d : |sed 's/[\x01-\x1F\x7F]//g'|sed 's%\[0m%%g'|sed 's%\[34m%%g'|sed ':a;N;$!ba;s/\n/ /g')
# loop over all runs
for runn in $runs; do
  # Only works on lxplus now!
  # if [[ ! -f "lbnevents_${runn}.txt" ]]; then 
  #   echo "Getting Nevent info for run $runn "
  #   LB_GetNEvents.py -r "$runn" -s "$sstream" | grep "... Run" | tee "lbnevents_${runn}.txt"
  # fi
  rm -f runs_temp_temp.txt
  touch runs_temp_temp.txt
  if [[ ! -f "runs_lbn_files.txt" ]]; then 
    echo "Getting file info for run $runn "
    file=$(rucio list-file-replicas "${pref}.00${runn}.${sstream}.${suf}.RAW" || true)
    # dataset=$(rucio list-dataset-replicas "${pref}.00${runn}.${sstream}.${suf}.RAW")
    set +e
    rsefound=$(echo "$file" | grep -c "CERN-PROD_TZDISK")
    if [[ ! $rsefound -eq 0 ]]; then
      echo "$file" | (grep "CERN-PROD_TZDISK" || true) | cut -d '|' -f 6 | sed "s% CERN-PROD_TZDISK: gsiftp://eosatlassftp.cern.ch:2811/%root://eosatlas.cern.ch//%" | sed -r 's/\s+//g' | grep "_lb" > runs_temp_temp.txt
      cat runs_temp_temp.txt >> runs_temp.txt
      # echo -n "Replicated? : "; (echo "$dataset" | grep CERN-PROD_TZDISK) || echo "false"
    fi
    nfound=$(grep -c data runs_temp_temp.txt)
    echo "Found $nfound files on TZDISK"
    set -e
    if [ "$nfound" -eq 0 ]; then
      # look also on DATADISK if not found on TZDISK
      set +e
      rsefound=$(echo "$file" | grep -c "CERN-PROD_DATADISK")
      if [[ ! $rsefound -eq 0 ]]; then
        echo "$file" | grep "CERN-PROD_DATADISK" | cut -d '|' -f 6 | sed "s% CERN-PROD_DATADISK: gsiftp://eosatlassftp.cern.ch:2811/%root://eosatlas.cern.ch//%" | sed -r 's/\s+//g' | grep "_lb" > runs_temp_temp.txt
        cat runs_temp_temp.txt >> runs_temp.txt
        # echo -n "Replicated? : "; (echo "$dataset" | grep CERN-PROD_DATADISK) || echo "false"
      fi
      nfound=$(grep -c data runs_temp_temp.txt)
      echo "Found $nfound files on DATADISK"
      set -e
    fi
    if [ "$nfound" -eq 0 ]; then
      # look also on RAW if not found on DATADISK
      set +e
      rsefound=$(echo "$file" | grep -c "CERN-PROD_RAW")
      if [[ ! $rsefound -eq 0 ]]; then
        echo "$file" | grep "CERN-PROD_RAW" | cut -d '|' -f 6 | sed "s% CERN-PROD_RAW: %%" | sed -r 's/\s+//g' | grep "_lb" > runs_temp_temp.txt
        cat runs_temp_temp.txt >> runs_temp.txt
        # echo -n "Replicated? : "; (echo "$dataset" | grep CERN-PROD_RAW) || echo "false"
      fi
      nfound=$(grep -c data runs_temp_temp.txt)
      echo "Found $nfound files on RAW"
      set -e
    fi
    if [ "$nfound" -eq 0 ]; then
      echo "No files found!"
    fi
  fi
done
rm runs_temp_temp.txt

if [[ ! -f runs_lbn_files.txt ]]; then mv runs_temp.txt runs_lbn_files.txt ; fi

# if you want to redo this:
# rm -f lbn_anal_map.txt 
if [[ ! -f lbn_anal_map.txt ]]; then LB_analyze | tee log_lbn_analyze.txt ; fi

# Only works on lxplus now!
# echo -n "Total events in dataset before GRL: "
# cat lbnevents_*.txt | tr -s ' ' | cut -d ' ' -f 8 | awk '{total = total + $1}END{print total}'
echo -n "Total events in dataset from log_lbn_analyze.txt: "
grep "stream 0" log_lbn_analyze.txt | cut -d ' ' -f 6 | awk '{total = total + $1}END{print total}'
echo -n "Total events in dataset from lbn_anal_map: "
grep "stream 1," lbn_anal_map.txt | cut -d ' ' -f 8 | awk '{total = total + $1}END{print total}'
echo -n "Selected events per stream: "
grep "stream 8," lbn_anal_map.txt | cut -d ' ' -f 17 | awk '{total = total + $1}END{print total}'

maxstream=19 #0 up to 49 - 19 does 1M events
#split into all the desired streams
for s in $(seq 0 $maxstream); do 
  grep "stream ${s}," lbn_anal_map.txt | grep -v "and 0 wanted" > "lbn_anal_map_stream${s}.txt"
  mkdir -p "output_stream${s}"
done

# split into jobs for each stream and make filelist and lbn_anal_map files
rm -f output_stream*/filelist_*.txt output_stream*/lbn_anal_map_*.txt
LB_AnalMapSplitter.py $maxstream | tee log_lbn_anal_map_splitter.txt
rm output_stream*/*501.txt
if grep "doesnt contain" log_lbn_anal_map_splitter.txt; then exit; fi
set +e
count=$(grep -c "not in files map" log_lbn_anal_map_splitter.txt)
set -e
echo "$count files not found in map..."

for s in $(seq 1 $maxstream); do
 echo "renaming stream $s to 0"
 for f in {1..500}; do sed -i -e "s%stream $s,%stream 0,%g" "output_stream${s}/lbn_anal_map_${f}.txt"; done
done

for s in $(seq 0 $maxstream); do cd "output_stream${s}"; tar cfz "stream${s}_${out_name}.tar.gz" *.txt; cd ..; done
mkdir -p lists
mv -v output_stream*/stream*_${out_name}.tar.gz lists/

#to check the events in each map:
for s in $(seq 0 $maxstream); do
 echo -n "files with 100 events in stream $s :"
 for f in {1..500}; do cat "output_stream${s}/lbn_anal_map_$f.txt" | cut -d ' ' -f 17 | awk '{total = total + $1}END{print total}'; done | grep -c 100
done

#to show number of files in each filelist:
echo ; echo "files in each filelist (in stream 0)..." ; for f in output_stream0/filelist_*.txt; do echo -n $(grep -o "$sstream" "$f" | wc -l); echo " : $f"; done | sort -n

echo "DONE"
