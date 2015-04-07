#!/bin/bash

#defaults
test="test12p3"; #the dataset made by "zerobias_grid.sh"
outset=3; #for the "testX" on the end of the merged dataset name
startstream=0
nstream=50
makeinfo="true"; untar="true";
checkevents="false";
makestreams="true";
domerging="true";
doregister="false"; dorealregistering="true";

function usage
{
    echo "usage: zerobias_merge.sh [ --test test --outset outset -s startstream -n nstream --makeinfo true|false --untar true|false --checkevents true|false --makestreams true|false --domerging true|false --doregister true|false dorealregistering true|false --help]"
}

while [ "$1" != "" ]; do
    case $1 in
        -t | --test )           shift; test=$1
                                ;;
        -o | --outset )    shift; outset=$1
                                ;;
        -s | --startstream )  shift; startstream=$1
                                ;;
        -n | --nstream )    shift; nstream=$1
                                ;;
        --makeinfo )    shift; makeinfo=$1
                                ;;
        --untar )    shift; untar=$1
                                ;;
        --checkevents )    shift; checkevents=$1
                                ;;
        --makestreams )    shift; makestreams=$1
                                ;;
        --domerging )    shift; domerging=$1
                                ;;
        --doregister )    shift; doregister=$1
                                ;;
        --dorealregistering )     shift; dorealregistering=$1
                                           ;;
        -h | --help )           usage
                                exit
                                ;;
        * )                     usage; exit 69
    esac
    shift
done

#do things in the temp directory
oldd=`pwd`
tmp="$HOME/nfs3/zerobias_skim_$test"
mkdir -p $tmp
cd $tmp ; pwd

zerods="group.dataprep.BSmultipleselector.GRL.$test"
let maxstream=$startstream+$nstream
echo "Doing $nstream streams, starting with stream $startstream ( maxstream is $maxstream )"
echo "makeinfo=$makeinfo untar=$untar checkevents=$checkevents makestreams=$makestreams domerging=$domerging doregister=$doregister zerods=$zerods outset=$outset "

if [ $makeinfo == "true" ]; then
if [ $untar == "true" ]; then 
 voms-proxy-info -exists
 if [ $? -gt 0 ]; then echo "Do voms.sh"; exit; fi
 dq2-get -T 5,5 --ignore ${zerods}/
 #rm run_* ; rm warn_* ;
fi
for d in `ls -d ${zerods}* `; do 
 #if [ $d == "user.haastyle.BSmultipleselector.GRL.test9.111215101153" ]; then echo "$d had bad transfer, being skipped"; continue; fi
 cd $d ; if [ $? -ne 0 ]; then pwd; echo "Directory $d did not exist"; continue; else pwd; fi
 if [ $untar == "true" ]; then for f in `ls group.dataprep.*.tgz*`; do echo $f; tar xzf $f; done; fi
 for j in `ls -d tarball_PandaJob_*`; do pwd; echo $j; 
  cd $j ; if [ $? -ne 0 ]; then pwd; echo "Directory $j did not exist"; continue; else pwd; fi
  grep "Job successfully completed" pilotlog.txt 
  if [ $? -eq 0 ]; then echo "Good job"; 
   run=`grep "start of run" athena_stdout.txt |head -1|cut -d ' ' -f 12`
   id=`ls jobState-*|cut -d '-' -f 2`
   echo "first run for job $id is $run"
   fil="../../run_${run}_${id}_${j}.txt"
   if [ -e "$fil" ]; then echo "$fil already exists"
   else
    grep "number of events written" athena_stdout.txt > tempev${1}.txt
    if [ `grep -c "number of events written" tempev${1}.txt ` -eq 50 ]; then
     for jf in `grep RAW OutPutFileCatalog.xml |cut -d \" -f 4`; do #echo "JF is $jf "
      stream=`echo $jf | cut -d '.' -f 8 | sed s%stream%% `; #echo "Stream is $stream  "
      nev=0 ; nev=`grep "BSESOutputSvc${stream}\ " tempev${1}.txt | cut -d ':' -f 2 `; #echo "NEv is $nev "
      echo "${jf}:${nev}" >> $fil
     done
     echo "$fil updated"
    else echo "Cant find number of events written"
    fi; rm tempev${1}.txt
   fi

   filw="../../warn_${run}_${id}_${j}.txt"
   if [ -e $filw ]; then echo "$filw already exists"
   else
    grep "OutputStreamBS" athena_stdout.txt | grep "expected" > $filw
   fi

  else echo "Failed job"
  fi
  cd .. ; pwd; echo "-----"
 done #loop over tarball_PandaJob* directories
 #rm -rf tarball_PandaJob_*
 cd .. ; pwd; echo; echo "=========="; echo
done #loop over group.* directories
fi #makeinfo

#check for errors with:
#cat log.txt | grep -v "done: 1 time"|grep -v skipping|grep -v GOOD
#and then full logfile with:
#less group.dataprep.BSmultipleselector.GRL.test*/tarball_PandaJob_1590664535_ANALY_LONG_BNL_ATLAS/athena_stdout.txt
if [ $checkevents == "true" ]; then
 if [ -e lbn_anal_map_temp.txt ]; then echo "lbn_anal_map_temp.txt exists"; 
 else cat lbn_anal_map.txt | grep "stream 0,"  > lbn_anal_map_temp.txt
 fi
while read myLine ; do echo -n $myLine ; 
run=`echo $myLine | cut -d " " -f 4 | sed s%,%% `
lbn=`echo $myLine | cut -d " " -f 6 | sed s%,%% `
nev=`echo $myLine | cut -d " " -f 8 `
echo -n " - Run LBN nev : $run $lbn $nev "
if [ $nev -eq 0 ]; then echo " - skipping "; continue; fi

grep " $run $lbn " warn_${run}* > tt.txt
if [ $? -ne 0 ]; then echo -n " - searching all"; grep " $run $lbn " warn_* > tt.txt ; 
 if [ $? -ne 0 ]; then echo " - not found";  continue; fi; 
fi

n=`grep -c INFO tt.txt `
echo -n " - done: $n time(s)"
if [ $n -gt 0 ]; then echo; continue; fi

n=`grep -c WARNING tt.txt `
if [ $n -gt 0 ]; then
 thetotal=`cat tt.txt | head -1 | cut -d " " -f 9 `
 foundtotal=`cat tt.txt | cut -d " " -f 5 | awk '{total = total + $1}END{print total}' `
 echo -n " : found $foundtotal out of $thetotal events"
 if [ $foundtotal -eq $thetotal ]; then echo " -- GOOD"; else echo " -- BAD"; fi
else 
 echo " : no warning "
fi

rm tt.txt
done < lbn_anal_map_temp.txt
fi #checkevents

if [ $makestreams == "true" ]; then
for (( targetstream=$startstream; targetstream<$maxstream; targetstream+=1 )); do 
rm stream-$targetstream-*filelist.txt
maxneperjob=2000; neperjob=$maxneperjob; tottot=0; tot=0; nfiles=0; skip=0; filelist="";
runtext=""; greptext="stream${targetstream}\.";
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
 if [ $tottot -le 51000 ]; then 
  echo $filelist > stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt ; 
  echo "Wrote stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt with $nfiles files"
 fi
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
 if [ $tottot -le 51000 ]; then 
  echo $filelist > stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt ; 
  echo "Wrote stream-${targetstream}-tot-${tottot}-skip-${skip}-ne-${neperjob}-filelist.txt with $nfiles files"
 fi
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

if [ $domerging == "true" ]; then echo
justonejob="false"; #"true" to do just one job, for a build job, or "skipfirst" to skip the first job, after a build job
tarball="--outTarBall ${tmp6dir}/tarball${startstream}.tgz  --outRunConfig ${tmp6dir}/runconf${startstream}.txt"
for (( targetstream=$startstream; targetstream<$maxstream; targetstream+=1 )); do echo 
 for c in `ls $tmp/stream-${targetstream}-*-filelist.txt` ; do echo $c
  if [ $justonejob == "skipfirst" ]; then echo "Skipping first file "; justonejob="false"; continue; fi
  jstream=`echo $c | cut -d '-' -f 2 `
  jtot=`echo $c | cut -d '-' -f 4 `
  jskip=`echo $c | cut -d '-' -f 6 `
  jne=`echo $c | cut -d '-' -f 8 `
  let maxe=${jne}+${jskip}
  let nout=${jne}/100
  uniq=`date +%s `; uniq="${uniq}_${test}_dataset${targetstream}"
  echo "noutputs is $nout since jne is $jne "
  inoutds="--inDS ${zerods}_EXT${jstream}/ --outDS `echo ${zerods}|sed s%user.haastyle.%group.dataprep.%g`_EXT${jstream}.merge.${jtot}.test${outset}"
  echo "inoutds is $inoutds"
  command=""; for (( ci=0; ci<${nout}; ci+=1 )); do command="$command outputbsfilterfile${ci}=%OUT.Zerobias${uniq}.bit-1.stream${ci}.RAW"; done
  echo "Doing jstream $jstream and jtot $jtot and jskip $jskip and maxe $maxe and uniq is $uniq "
  catc=`cat $c`
  echo "Filelist is $catc "
  #if [ $justonejob != "true" ]; then lib="--libDS user.haastyle.0328175735.886361.lib._005840"; fi; #17.0.6.5
  lib="--noBuild";
  #site="--site ANALY_CERN_XROOTD"
  site="--site ANALY_BNL_ATLAS_1"
  let jobsetid=$outset*100+100000+$targetstream ; jobset="--panda_jobsetID $jobsetid " ; echo "jobset is $jobset "
  ~/bin/pathena --trf "BS_multipleSelector_trf.py inputbsfile=%IN $command lbnmapfile=serial TriggerBit=-1 Noutputs=$nout maxevents=$maxe skipevents=$jskip uniq=$uniq " --tmpDir ${tmp6dir}/ $inoutds --fileList $catc --memory=1800 --split=1 --dbRelease ddo.000001.Atlas.Ideal.DBRelease.v170601:DBRelease-17.6.1.tar.gz --skipScan $tarball $lib $jobset --official "--voms=atlas:/atlas/dataprep/Role=production" ${site} 
  #--noSubmit -v 
  rt=$? ; if [ $rt -ne 0 ]; then echo "return value $rt from pathena for $c" > fail_$c ; fi
  tarball="--inTarBall ${tmp6dir}/tarball${startstream}.tgz --inRunConfig ${tmp6dir}/runconf${startstream}.txt"
  echo
  if [ $justonejob == "true" ]; then echo "just doing one job"; break; fi
 done
done
fi #domerging

#../zerobias_merge.sh -s 1 -n 4 -t test12p3 -o 3 --makeinfo false --untar false --checkevents false --makestreams false --domerging false --doregister true
if [ "$doregister" == "true" ]; then echo
for (( targetstream=$startstream; targetstream<$maxstream; targetstream+=1 )); do echo 

    #dq2-delete-datasets group.dataprep.BSmultipleselector.GRL.test9_EXT43.merge.test14/ `dq2-list-datasets-container group.dataprep.BSmultipleselector.GRL.test9_EXT43.merge.test14/`

    dname="`echo ${zerods}|sed s%user.haastyle.%group.dataprep.%g`_EXT${targetstream}.merge"
    outdname="${dname}.test${outset}/"
    if [ $dorealregistering == "true" ]; then dq2-register-container "${outdname}"; fi
 
    outsetin="${dname}.*.test${outset}/"
    #outsetin="user.haastyle.BSmultipleselector.GRL.test9_EXT0.merge.*.test12/"
    echo "Taking datasets from $outsetin"; echo

    if [ $dorealregistering == "true" ]; then
    for dc in `dq2-ls "${outsetin}"`; do echo "dataset container is ${dc}";
	for dd in `dq2-list-datasets-container ${dc}`; do dq2-register-datasets-container $outdname "${dd}"; echo $dd; done
	#dq2-register-datasets-container $outdname `dq2-list-datasets-container ${dc}`
    done; fi

    echo "${outdname} now has `dq2-list-datasets-container ${outdname} |grep -c merge` datasets and `dq2-ls -f -H ${outdname} |grep -c RAW` RAW files and `dq2-ls -f -H ${outdname} |grep \"total size:\"`"

    #check for empty files
    dq2-ls -f -H $outdname |grep Zerobias > temp92.txt
    grep "bytes" temp92.txt > temp93.txt
    if [ $? -eq 0 ]; then echo "Dataset $outdname has empty files: "; cat temp93.txt; 
    else echo "Dataset $outdname has no empty files"; fi
    rm temp92.txt temp93.txt

done
fi

