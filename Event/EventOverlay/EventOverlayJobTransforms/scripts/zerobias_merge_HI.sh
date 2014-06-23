#!/bin/bash

outset=2; #for the "testX" on the end of the merged dataset name - and also modifies the jobset id

startstream=99
maxstream=100
echo "Doing 1 stream, starting with stream $startstream ( maxstream is $maxstream )"

#do things in the temp directory
oldd=`pwd`
tmp="$HOME/nfs3/HI_minbias_skim"
mkdir -p $tmp
cd $tmp ; pwd

makeinfo="f"
makestreams="f"
domerging="f"
justonejob="f"; #"t" to do just one job, for a build job, or "skipfirst" to skip the first job, after a build job
doregister="t"

#prefix="data11_hi"
prefix="data12_hip"

if [ $makeinfo == "t" ]; then
rm runHI_*

#for r in 194374 194370 194193 194192 194179 194163 194160 194121 194061 194060 194017 193890 193834 193826 193825 193823 193795 193718 193687 193679 193662 193655 193641 193604 193599 193558 193546 193494 193493 193492 193491 193481 193463 193447 193412 193403 193321 193295 193291 193270 193211 194382 ; #2011 Pb-Pb
for r in 210184 ; #2011 p-Pb

 do echo "Run is $r";
 if [ -e dq2_${r}.txt ]; then echo "dq2_${r}.txt already exists"; else dq2-ls -f ${prefix}.00${r}.physics_MinBiasOverlay.merge.RAW >& dq2_${r}.txt; fi
 if [ -e nevent_${r}.txt ]; then echo "nevent_${r}.txt already exists"; else python $oldd/GetNEventsLB.py --run $r --stream physics_MinBiasOverlay | sed "s%\ %%g" >& nevent_${r}.txt; fi
 for f in `grep SFO dq2_${r}.txt |cut -f 2`; do echo $f
  lb=`echo $f | cut -d . -f 6 | sed 's%_lb%%' | sed 's/^[0]*//' `;
  sfo=`echo $f | cut -d . -f 7| sed 's%_SFO-%%'`; echo "lbn is $lb and sfo $sfo "
  ne=`grep "LB${lb}has" nevent_${r}.txt | cut -d e -f 1 |cut -d s -f 2`; 
  if [ $sfo != "ALL" ]; then 
   let ne=ne/10; #since there's 10 SFO's per lbn, at least for 2012 data...
  fi
  echo "ne is $ne"
  echo "${f}:${ne}">runHI_${r}_${lb}_${sfo}_${ne}.txt
 done
done
fi #makeinfo

if [ $makestreams == "t" ]; then
for (( targetstream=$startstream; targetstream<$maxstream; targetstream+=1 )); do 
#if [ `ls stream-$targetstream-*|grep -c stream ` -eq 50 ]; then continue; fi
rm stream-$targetstream-*filelist.txt
maxneperjob=2000
neperjob=$maxneperjob
tottot=0
tot=0
nfiles=0
skip=0
filelist=""
runtext="HI"; greptext="MinBiasOverlay";
for r in run${runtext}_*.txt ; do echo $r
f=`grep $greptext $r | cut -d ':' -f 1 `
ne=`grep $greptext $r | cut -d ':' -f 2 |sed "s%\ %%g"`
echo "file $f has $ne events"; 
if [ $ne -eq 0 ]; then continue; fi
let tot+=$ne
let tottot+=$ne
echo "Total events is $tot and totaltotal is $tottot and neperjob is set to $neperjob and nfiles is $nfiles "
echo
if [ $tot -ge $neperjob ]; then 
 if [ "$filelist" != "" ]; then filelist="${filelist},"; fi
 filelist="${filelist}${f}"
  echo $filelist > stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt ; 
  echo "Wrote stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt with $nfiles files"
 let skip=$ne-$tot+$neperjob
 if [ $skip -eq $ne ]; then filelist=""; skip=0; #no reason to add the whole file and skip through the whole thing
 else filelist="${f}"
 fi
 let tot=$tot-$neperjob
 let nfiles=0
 let neperjob=$maxneperjob
 echo "Skip set to $skip and filelist is now $filelist and tot is $tot and tottot is $tottot "
 echo
else
 if [ "$filelist" != "" ]; then filelist="${filelist},"; fi
 filelist="${filelist}${f}"
 let nfiles=$nfiles+1
 if [ $nfiles -gt 100 ]; then  #check if we need to cut it off short to stay below 200 file limit
  oldneperjob=$neperjob ; let neperjob=$tot+100 ; let neperjob=$neperjob/100*100 ;
  if [ $oldneperjob -ne $neperjob ]; then echo "neperjob now set to $neperjob"; fi
 fi
fi #tot -ge neperjob
done #for each run_*.txt file 
if [ "$filelist" != "" ]; then 
 #echo "WARNING, filelist not empty at the end: "; echo $filelist ; 
 oldneperjob=$neperjob ; let neperjob=$tot+100 ; let neperjob=$neperjob/100*100 ;
 if [ $oldneperjob -ne $neperjob ]; then echo "neperjob now set to $neperjob"; fi
  echo $filelist > stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt ; 
  echo "Wrote stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt with $nfiles files"
fi
echo "Done with stream $targetstream and TotalTotal is $tottot events "
echo
done #for each targetstream
fi

#go back to our original directory
cd $oldd ; pwd

tmp6dir="dummy"
if [ -e /scratch/ahaas ]; then tmp6dir="/scratch/ahaas"; fi
if [ -e /tmp/ahaas ]; then tmp6dir="/tmp/ahaas"; fi
mkdir -p $tmp6dir

if [ $domerging == "t" ]; then echo
tarball="--outTarBall ${tmp6dir}/tarball${startstream}.tgz  --outRunConfig ${tmp6dir}/runconf${startstream}.txt"
for (( targetstream=$startstream; targetstream<$maxstream; targetstream+=1 )); do echo 
 for c in `ls $tmp/stream-${targetstream}-*-filelist.txt` ; do echo $c
  if [ $justonejob == "skipfirst" ]; then echo "Skipping first file "; justonejob="f"; continue; fi
  jstream=`echo $c | cut -d '-' -f 2 `
  jtot=`echo $c | cut -d '-' -f 4 `
  jskip=`echo $c | cut -d '-' -f 6 `
  jne=`echo $c | cut -d '-' -f 8 `
  let maxe=${jne}+${jskip}
  let nout=${jne}/100
  uniq=`date +%s `; uniq="${uniq}q"
  echo "noutputs is $nout since jne is $jne "
   hiinds=""
   for run2 in `cat $c | sed "s%,%\n%g" | cut -d . -f 2`; do 
    if [[ "$hiinds" == *"$run2"* ]]; then continue
    else
     if [ "$hiinds" != "" ]; then hiinds="${hiinds},"; fi
     hiinds="${hiinds}${prefix}.${run2}.physics_MinBiasOverlay.merge.RAW";
    fi
   done
   inoutds="--inDS $hiinds --outDS group.dataprep.${prefix}.HImerge.physics_MinBiasOverlay.${jtot}.test$outset"
  echo "inoutds is $inoutds"
  command=""; for (( ci=0; ci<${nout}; ci+=1 )); do command="$command outputbsfilterfile${ci}=%OUT.Zerobias${uniq}.bit-1.stream${ci}.RAW"; done
  echo "Doing jstream $jstream and jtot $jtot and jskip $jskip and maxe $maxe and uniq is $uniq "
  catc=`cat $c`
  echo "Filelist is $catc "
  if [ $justonejob != "t" ]; then lib="--libDS user.haastyle.1012174230.597093.lib._014565"; fi;
  #if [ $justonejob != "t" ]; then lib="--noBuild"; fi
  #site="--site ANALY_CERN_XROOTD"
  #site="--site ANALY_BNL_ATLAS_1"
  let jobsetid=$outset*100+100000+$targetstream ; jobset="--panda_jobsetID $jobsetid " ; echo "jobset is $jobset "
  pathena --trf "BS_multipleSelector_trf.py inputbsfile=%IN $command lbnmapfile=serial TriggerBit=-1 Noutputs=$nout maxevents=$maxe skipevents=$jskip uniq=$uniq " --tmpDir ${tmp6dir}/ $inoutds --fileList $catc --memory=1800 --split=1 --dbRelease ddo.000001.Atlas.Ideal.DBRelease.v170601:DBRelease-17.6.1.tar.gz --skipScan $tarball $lib $jobset --official "--voms=atlas:/atlas/dataprep/Role=production" ${site} 
  rt=$? ; if [ $rt -ne 0 ]; then echo "return value $rt from pathena for $c" > fail_`basename $c` ; fi
  tarball="--inTarBall ${tmp6dir}/tarball${startstream}.tgz --inRunConfig ${tmp6dir}/runconf${startstream}.txt"
  echo
  if [ $justonejob == "t" ]; then echo "just doing one job"; break; fi
 done
done
fi #domerging

if [ "$doregister" == "t" ]; then echo
for (( targetstream=$startstream; targetstream<$maxstream; targetstream+=1 )); do echo 
     dname="group.dataprep.${prefix}.HImerge.physics_MinBiasOverlay";
     for (( targetstr=0; targetstr<10; targetstr+=1 )); do dq2-register-container "${dname}_EXT${targetstr}.test${outset}/"; done
 
 outsetin="${dname}.*.test${outset}/"
 #outsetin="user.haastyle.HImerge.193825p.test12/"
 echo "Taking datasets from $outsetin"

 dcx=0;
 for dc in `dq2-ls "${outsetin}"`; do echo "dcx is $dcx and dataset container is ${dc}";
      dcxmod=$(($dcx % 10)); #break HI dataset into 10 streams
      outdname="${dname}_EXT${dcxmod}.test${outset}/"
      echo "selecting output dataset ${outdname}"
      let dcx=dcx+1
  #for dd in `dq2-list-datasets-container ${dc}`; do dq2-register-datasets-container $outdname "${dd}"; done
  dq2-register-datasets-container $outdname `dq2-list-datasets-container ${dc}`
 done
 
 for (( targetstr=0; targetstr<10; targetstr+=1 )); do
     outdname="${dname}_EXT${targetstr}.test${outset}/"
     echo "${outdname} now has `dq2-list-datasets-container ${outdname} |grep -c merge` datasets and `dq2-ls -f -H ${outdname} |grep -c RAW` RAW files and `dq2-ls -f -H ${outdname} |grep \"total size:\"`"
 done
 
done
fi

