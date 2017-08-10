#!/bin/bash

if [ ! -e log.* ]
then
    exit
fi

fillog=$(echo log.*)

numevini=$(grep "Events processed:" $fillog | awk '{print $6}')
ename2=$(ls *_log | awk -F "_" 'NR==1 {print $1}')

outfile1=DerivationSummary_${ename2}.txt
outfile4=ErrorsWarningsSummary_${ename2}.txt

nevt=$(grep "INFO  Stream StreamDAOD_" $fillog | awk '{ print $6}')
skimfr=$(echo "$nevt/$numevini" | bc -l)
skimfr2=$(printf '%.6f\n' $skimfr)

inpaodfile=$(grep "INFO opening" $fillog | awk '{ print $5}' | grep "root" | awk -F "//" '{print $3}'  | awk -F "]..." '{print $1}')

inaodsize1=$(/afs/cern.ch/project/eos/installation/atlas/bin/eos.select ls -l /$inpaodfile | awk '{print $5}')
inaodsize=$(($inaodsize1/1024))

filesizes=tempfile2.txt
ls -s -1 *.pool.root > $filesizes

filnam=$(awk 'NR==1 {print $2}' $filesizes)
nomsize=$( awk 'NR==1 {print $1}' $filesizes)
fraod1=$(echo "$nomsize/$inaodsize" | bc -l)
fraod2=$(printf '%.6f\n' $fraod1)
dvnam=$(echo $filnam | awk -F "_" '{ print $2}' | awk -F "." '{ print $1}')
if [ -e log.ESDtoDPD ]
then
    dvnam=$(grep -m 1 "Stream Stream" log.ESDtoDPD | awk '{print $4}' | awk -F"_" '{print $2}')
fi
echo $dvnam
humsize=$(ls -sh $filnam | awk '{ print $1 }')

cpuconsum=$(grep -A1 "Statistics for 'evt'" $fillog | grep "<cpu>" | awk '{print $6" "$10}')
vmem=$(grep -A3 "vmem-leak estimation" $fillog | grep "last" | awk '{print $7" "$8}')
vmemleak=$(grep "fitted vmem-slope" $fillog | grep "evt 101+" | awk '{print $10" "$11}')

nevtsproc=$(grep "Events processed" $fillog | awk '{print $6}')

tjtchrono=$(grep -E 'ChronoStatSvc .*Time' $fillog | awk '{print $8 " " $9}')

echo " " >> $outfile1
echo -e "Car      SizeOnDisk          SizeFractionFromAOD      SkimFraction          CPU          VMEM           MemLeak     EvtsProcessed  TotalJobTime(Chronostat)      NoEvtsInFile" >> $outfile1
echo $dvnam"   "$humsize"                "$fraod2"                  "$skimfr2"            "$cpuconsum"     "$vmem"      "$vmemleak"      "$nevtsproc "   "$tjtchrono"       "$nevt>>$outfile1
echo "  " >> $outfile1
echo " " >> $outfile1
echo -e "NB: Skimming fraction is computed using the number of  analyzed events." >> $outfile1
echo -e "NB: The input AOD file has only 5000 events, the size and the fractions are computed with respect to it, not to the whole AOD file. Also, the size of the derivation is smaller." >> $outfile1

echo -e "Errors, warnings, fails etc." >> $outfile4
grep -i -e "WARNING" -e "error" -e "fail" $fillog >> $outfile4

rm $filesizes
