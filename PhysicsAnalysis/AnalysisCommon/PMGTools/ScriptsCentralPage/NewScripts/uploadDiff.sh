#!/bin/bash                                                                                                                                                                      
#***************************************************************************************************************************************************                             
export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
source /cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase/user/atlasLocalSetup.sh --quiet
asetup AthAnalysisBase,2.4.16,here
localSetupPyAMI
3PROXY=/tmp/x509up_u`id -u`
voms-proxy-init -valid 720:00 -out $PROXY
chmod 400 $PROXY
chmod 777 /tmp/x509up_u28230
export MYPROXY_SERVER=myproxy.cern.ch
myproxy-init -d -n -c 900 --voms atlas
voms-proxy-init -voms atlas
basename=`echo $1`
substrback="_Download.txt"
basename=`echo ${basename%$substrback}`
substrfront="list_Xsec_"
basename=`echo ${basename#*"$substrfront"}`
echo ${basename}
datetime="`date +20%y-%m-%d_%H%M`"
echo ${datetime}
outCSV=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/csvFiles/PMGvsAMI_${basename}_${datetime}.csv
outDiffLog=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/diffLogs/diff_PMGvsAMI_${basename}_${datetime}.log
source GetMetadataFromAMI.sh ${basename} ${outCSV} 
printstr=`echo ${outCSV} ${outDiffLog}`
(exec python DiffPMGAMI.py $printstr)>&1
source SetupPMGtoAMI.sh ${outDiffLog}