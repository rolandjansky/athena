#!/bin/sh
# This script produces a BS file from Z->ee , Z->mumu and Enhanced Bias events
# all from an Enhanced Bias run (default is 202798 as May 2012). 
# the output file will be in directory : /tmp/$USER 
# usage :   atn_makeTestFile.sh  <run-number> 
# output file will contain 99 events, 33 of each type
# in order to change the number and type of events in the output file look at line 11 of TrigValTools/bin/dumpL1Events.py ("pasEvt > 32")     

defrun="202798"
runnumber=$1
##Change the following lines when changing to new release or new LVL1 menu  
rel_proj="AtlasP1HLT"
rel_numb="17.1.5.3"
xmlL1="/afs/cern.ch/atlas/software/builds/${rel_proj}/${rel_numb}/InstallArea/XML/TriggerMenuXML/LVL1config_Physics_pp_v4.xml"
#on lxplus, move to /tmp/user directory
mkdir /tmp/$USER
cd /tmp/$USER
#latest EB runnumber 202798
if [ -n $runnumber ]; then 
    echo  "using default run number $defrun"
    runnumber=$defrun
fi

#setting STAGE_SVCCLASS=atlcal for retrieving CosmicCalo data from castor 
export STAGE_SVCCLASS=atlcal
echo "setting STAGE_SVCCLASS to $STAGE_SVCCLASS" 

#make sure you have grid access with DQ2:
source /afs/cern.ch/atlas/offline/external/GRID/ddm/DQ2Clients/setup.sh 
voms-proxy-init -voms atlas 
##get the files for a particular run 
echo starting to get Zee and Zmumu files for run $runnumber 
dq2-get -n 1  data12_8TeV.00${runnumber}.physics_Egamma.merge.DRAW_ZEE*
dq2-get -n 1  data12_8TeV.00${runnumber}.physics_Muons.merge.DRAW_ZMUMU*
echo dq2 done with  Zee and Zmumu files for run $runnumber
dq2-get -n 1 data12_8TeV.00${runnumber}.physics_EnhancedBias.merge.RAW
echo dq2 done with EnhancedBias file for run $runnumber

export AtlasSetup=/afs/cern.ch/atlas/software/dist/AtlasSetup
source $AtlasSetup/scripts/asetup.sh ${rel_proj},${rel_numb}
echo ${AtlasPatch},${AtlasVersion}
echo ${rel_proj},${rel_numb}

zeeFile=`ls -1 data12_8TeV.00${runnumber}.physics_Egamma.merge.DRAW_ZEE.*/*.1` 
zmumuFile=`ls -1 data12_8TeV.00${runnumber}.physics_Muons.merge.DRAW_ZMUMU.*/*.1` 
ebFile=`ls -1 data12_8TeV.00${runnumber}.physics_EnhancedBias.merge.RAW/*.1` 

zeeEventlist=`dumpL1Events.py  $zeeFile  $xmlL1 |tail -1`
zeeDir=data12_8TeV.00${runnumber}.physics_ZEE.merge.RAW 
mkdir $zeeDir-tmp/
zeeOutFile="$zeeDir-tmp/$zeeDir"
trigbs_pickEvents.py $zeeFile  $zeeOutFile $zeeEventlist

zmumuEventlist=`dumpL1Events.py  $zmumuFile  $xmlL1 |tail -1`
zmumuDir=data12_8TeV.00${runnumber}.physics_ZMUMU.merge.RAW 
mkdir ${zmumuDir}-tmp/
zmumuOutFile="${zmumuDir}-tmp/${zmumuDir}"
echo trigbs_pickEvents.py $zmumuFile  $zmumuOutFile $zmumuEventlist

ebEventlist=`dumpL1Events.py  $ebFile  $xmlL1 |tail -1`
ebDir=data12_8TeV.00${runnumber}.physics_EnhancedBias.merge.RAW 
mkdir ${ebDir}-tmp/
ebOutFile="${ebDir}-tmp/${ebDir}"
echo trigbs_pickEvents.py $ebFile  $ebOutFile $ebEventlist
baseName=`ls ${ebDir}/ | sed s/merge/merge_eb_zee_zmumu_cc/ | sed s/._0001.1//`

#Now prepare 6 events from CosmicCalo stream (empty triggers)  
#find LB from EB raw file:
lb=`ls ${ebDir} | cut -d"_" -f4 | cut -d"." -f 1` 
ccDir=data12_8TeV.00${runnumber}.physics_CosmicCalo.merge.RAW
mkdir ${ccDir}
mkdir ${ccDir}-tmp
ccCastor=/castor/cern.ch/grid/atlas/tzero/prod1/perm/data12_8TeV/physics_CosmicCalo/00${runnumber}/data12_8TeV.00${runnumber}.physics_CosmicCalo.merge.RAW
ccFile=`nsls $ccCastor | grep $lb`
rfcp ${ccCastor}/${ccFile} ${ccDir}/.
ccEventlist=`dumpL1Events.py  ${ccDir}/${ccFile}  $xmlL1 | tail -1 | grep -o "[0-9] [0-9] [0-9] [0-9] [0-9] [0-9] "` 
echo trigbs_pickEvents.py ${ccDir}/${ccFile} ${ccDir}-tmp/${ccFile} $ccEventlist

echo "making final file and renumbering LVL1 ids.............."
trigbs_mergeBSfiles.py -v ${baseName} ${ebDir}-tmp ${zeeDir}-tmp ${zmumuDir}-tmp ${ccDir}-tmp
echo "done with making file:"
ls -lrt ${baseName}*
exit
