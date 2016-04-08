#!/bin/bash

collections=("xAOD::Jet_v1" "xAOD::Truth*" "xAOD::TrackParticle_v1" "xAOD::Trig*" "xAOD::MissingET*" )

for f in $(\ls -1 *.root);
  do
  fLen=$(wc -l $f | awk '{print $1}')
  if [ "$fLen" == "0" ]; then printf "No DxAODs found\n\n"; unset f; break; fi
  echo "running checkxAOD.py $f"
  checkxAOD.py $f > collections.txt
  checkxAOD_output=collections.txt
  printf "\n"
  printf "*******************************************************************************\n"
  printf '%s\n' $f
  printf "*******************************************************************************\n"
  printf "All sizes in [kb/evt]\n"
  grep -m 1 "Total" $checkxAOD_output | awk '{print $5}' > test1.txt
  TotalSize=$(awk '{for(i=1;i<=NF;i++)s+=$i}END{print s}' test1.txt)
  printf '%-54s %.2f \n' "Total Derivation Size " $TotalSize
  for i in "${collections[@]}";
    do
    grep "$i" $checkxAOD_output | awk '{print $5}' > test2.txt
    size=$(awk '{for(i=1;i<=NF;i++)s+=$i}END{print s}' test2.txt)
    fraction=$(echo "($size/$TotalSize)*(100)" | bc -l)
    printf '%-54s %.2f (%.2f%% of total)\n' "$i " $size $fraction
    grep "$i" $checkxAOD_output | awk '{printf "     %-50s %s\n", $9, $5}' > test3.txt
    while read p
      do
      echo "    $p"
      done < test3.txt
    done
  grep "xAOD::" $checkxAOD_output > test4.txt
  for j in "${collections[@]}";
    do
    sed -i "/"$j"/d" test4.txt
    done
  grep "xAOD::" test4.txt | awk '{print $5}' > test5.txt
  size=$(awk '{for(i=1;i<=NF;i++)s+=$i}END{print s}' test5.txt)
  fraction=$(echo "($size/$TotalSize)*(100)" | bc -l)
  printf '%-54s %.2f (%.2f%% of total)\n' "xAOD::<Other>" $size $fraction
  grep "xAOD::" test4.txt | awk '{printf "     %-50s %s\n", $9, $5}' > test6.txt
  while read k
    do
    echo "    $k"
    done < test6.txt

  printf "*******************************************************************************\n"
  unset f
  rm -f collections.txt test1.txt test2.txt test3.txt test4.txt test5.txt test6.txt
done




