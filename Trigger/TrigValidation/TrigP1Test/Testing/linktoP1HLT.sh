#!/usr/bin/env bash
#
echo "=== linktoHLT.sh"
if [ $# -ge 1 ]; then
   type=$1
   echo "looking for files with ${type} in name" 
else
   type=""
fi
thisrel=`printenv CMTPATH | awk '{ print  substr($1, index($1,"builds/nightlies/")+17, 30 )  }' | awk '{print substr($1,1,index($1,"/Atlas")-1) }' ` 
echo "this release: $thisrel" 

# This step has to be adjusted by hand, when the baserelease for T0 reco and P1HLT cache are different
savedbsrel=$thisrel
echo "$savedbsrel" | grep 19.X.0
if [[ $? == 0 ]]
then 
 savedbsrel="${thisrel/19.X.0/19.0.X.Y}"  
fi


echo "rel of saved BS",$savedbsrel

echo "$savedbsrel" | grep T0
if [[ $? == 0 ]] 
then
  sname="/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/online/references/ATN/${savedbsrel}/latest/*${type}-*.data"
else
  sname="/afs/cern.ch/atlas/project/trigger/pesa-sw/validation/online/references/ATN/${savedbsrel}-T0/latest/*${type}-*.data"
fi
#
ls -lrt $sname 
latestfile=`ls -rt $sname | tail -1`
filesize=`stat -c%s $latestfile`
if [[ $? == 0 ]]
then 
  `ln -s ${latestfile} link_to_file_from_P1HLT.data`
  ls -l  link_to_file_from_P1HLT.data
else
  echo "ERROR: could not link to data file; test will end in FAILURE"
  echo "ERROR: check that links are correctly set at /afs/cern.ch/atlas/project/trigger/pesa-sw/validation/online/references/ATN/"
  exit 1
fi   
