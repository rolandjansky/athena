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
voms-proxy-init -voms atlas
sampleBunchName=`echo $1`
echo ${sampleBunchName}
outcsv=`echo $2`
FILE1=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/XsecFiles/list_Xsec_"$1"_Download.txt
cat $FILE1 | while read line
do
    str=`echo $line |awk '{print $1}'`
    if [ "$str" == "#" ]; then
        continue
    else
        DSID=`echo $line |awk '{print $1}'`
        subDSID=`echo "$str" | awk '{print substr($0,0,3)}'`
        sample=`echo $line |awk '{print $2}'`
	PMGAMIXSec=`echo $line |awk '{print $3}'`
        PMGBR=`echo $line |awk '{print $4}'`
        PMGFeff=`echo $line |awk '{print $5}'`
        PMGTotalHighXSec=`echo $line |awk '{print $6}'`
        PMGkfact=`echo $line |awk '{print $7}'`
        PMGHighXsec=`echo $line |awk '{print $8}'`
	echo "$sample" > my.datasets.txt
	getMetadata.py --inDsTxt=my.datasets.txt --outFile=metadata.txt
	info=`awk 'NR==2' metadata.txt`
	AMIXsec=`echo "$info" | awk '{print $2}'`
	AMIXsec=`echo "$info" | awk '{printf"%.10f", $2}'`
	kfact=`echo "$info" | awk '{print $3}'`
	Feff=`echo "$info" | awk '{printf"%.10f", $4}'`
	AMIXsec=`echo "$AMIXsec * 1000" | bc`
	TotXsec=`echo "$AMIXsec * $kfact * $Feff" | bc`
	printstr=`echo ${DSID} "," ${PMGAMIXSec} "," ${PMGkfact} "," ${PMGFeff} "," ${PMGHighXsec} "," ${AMIXsec} "," ${kfact} "," ${Feff} "," ${TotXsec} "," ${sample} "," ${PMGTotalHighXSec} "," ${outcsv}`
	echo ${printstr}
	(exec python print_file.py $printstr)>&1
    fi
done



