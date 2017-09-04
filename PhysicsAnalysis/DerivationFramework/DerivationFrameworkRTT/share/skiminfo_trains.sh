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


nevttemp=tmpfilnevts.txt
grep "INFO  Stream StreamDAOD_" $fillog >|  $nevttemp

##because the shell scripting doesn't behave well when it comes to dynamic arrays, I am writing some of them into a file and I will try to retrieve them when appropriate

declare -A skimfrr
idxx=0
skmtmp=skmtmpfile.txt

cat $nevttemp | while read line
do
    nevt=$(echo $line | awk '{print $6}')
    if [ $nevt -eq 0 ]
    then
	skimfrr[idxx]=0.0
	echo ${skimfrr[idxx]} >> $skmtmp
    else
	skimfr=$(echo "$nevt/$numevini" | bc -l)
	skimfrr[idxx]=$(printf '%.6f\n' $skimfr)
	echo ${skimfrr[idxx]} >> $skmtmp
    fi
    idxx=$(($idxx+1))
done

cat $skmtmp

rm $nevttemp

unstrevts=$(grep "Unstreamed events" $fillog | awk '{print $6}')
skimfr2=$(echo "$unstrevts/$numevini" | bc -l)
skimfr21=$(printf '%.6f\n' $skimfr2)

inpaodfile=$(grep -E "INFO opening.*root" $fillog | awk '{ print $5}' | grep "root" | awk -F "//" '{print $3}'  | awk -F "]..." '{print $1}')
inaodsize1=$(/afs/cern.ch/project/eos/installation/atlas/bin/eos.select ls -l /$inpaodfile | awk '{print $5}')
inaodsize=$(($inaodsize1/1024))

filesizes=tempfile2.txt
ls -s -1 DAOD*.pool.root > $filesizes
filesizes2=tmpfile22.txt
awk '{getline f1 <"tempfile2.txt" ; print f1" "$1}' < $skmtmp > $filesizes2
rm $skmtmp

declare -A dvnam
declare -A sizefrr
declare -A skmfr2
declare -A humsize
idxa=0

echo " " >> $outfile1
echo "Info about individual derivations" >>$outfile1
echo "Derivation    SizeFraction        SkimFraction    SizeOnDisk" >> $outfile1

cat  $filesizes2 | while read line
do
    dvnam[idxa]=$(echo $line | awk '{print $2}' | awk -F "_" '{ print $2}' | awk -F "." '{ print $1}')
    echo ${filnam[idxa]}
    nomsize=$(echo $line | awk '{print $1}')
    fraod1=$(echo "$nomsize/$inaodsize" | bc -l)
    sizefrr[idxa]=$(printf '%.6f\n' $fraod1)
    skmfr2[idxa]=$(echo $line | awk '{print $3}')
    filnam=$(echo $line | awk '{print $2}')
    humsize[idxa]=$(ls -sh $filnam | awk '{ print $1 }')    
    idxa=$(($idxa+1))
    echo ${dvnam[idxa]}"      "${sizefrr[idxa]}"        "${skmfr2[idxa]}"       "${humsize[idxa]} >> $outfile1
done

rm $filesizes
rm $filesizes2

cpuconsum=$(grep -A1 "Statistics for 'evt'" $fillog | grep "<cpu>" | awk '{print $6" "$10}')
vmem=$(grep -A3 "vmem-leak estimation" $fillog | grep "last" | awk '{print $7" "$8}')
vmemleak=$(grep "fitted vmem-slope" $fillog | grep "evt 101+" | awk '{print $10" "$11}')

tjtchrono=$(grep -E 'ChronoStatSvc .*Time' $fillog | awk '{print $8 " " $9}')

echo " " >> $outfile1
echo -e "Info about the train"
echo  -e "UnskimFraction          CPU          VMEM           MemLeak     EvtsProcessed  TotalJobTime(Chronostat)" >> $outfile1
echo $skimfr21"         "$cpuconsum"     "$vmem"     "$vmemleak"      "$numevini"         "$tjtchrono >>$outfile1
echo " " >> $outfile1
echo "  " >> $outfile1

echo -e "NB: UnskimFraction is the ratio between the unstreamed events and the number of processed events. It is computed only for trains." >> $outfile1
echo -e "NB: Skimming fraction is computed using the number of events present in the stream at the end of the process and the number of processed events. Please compare this number and the others specific to each derivation to those in the respective single carriage jobs." >> $outfile1
echo -e "NB: The input AOD file has only 5000 events, the size and the fractions are computed with respect to it, not to the whole AOD file. Also, the size of the derivation is smaller. Please keep in mind that some derivations run on a smaller number of events as requested by the respective groups." >> $outfile1

echo -e "Errors, warnings, fails etc." >> $outfile4
grep -i -e "WARNING" -e "error" -e "fail" $fillog >> $outfile4



