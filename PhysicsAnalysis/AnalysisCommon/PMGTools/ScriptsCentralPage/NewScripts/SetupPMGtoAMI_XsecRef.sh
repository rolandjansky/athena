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

FILE1=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/XsecFiles/list_Xsec_TTbarBulk_Download.txt

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
        BR=`echo $line |awk '{print $4}'`
        Feff=`echo $line |awk '{print $5}'`
        TotalXSec=`echo $line |awk '{print $6}'`
	if [ "$TotalXSec" == "Undefined" ]; then
            continue
	else
	    FILE2=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/InputInformation/CrossSectionReference
	    cat $FILE2 | while read line1
	    do
		highXsec=`echo $line1 |sed -e 's/\,/ /g' |awk '{print $2}'`
		if [ "$highXsec" == "$TotalXSec" ]; then
		    thRef=`echo $line1 |sed -e 's/\,/ /g' |awk '{print $1}'` 
		    kfact=`echo $line |awk '{print $7}'`
		    HighXsec=`echo $line |awk '{print $8}'`
		    echo "XsecRef" ${thRef}
		    echo "DSID" ${DSID} "sample" ${sample} "kfact" ${kfact} "thRef" ${thRef}
		    ami cmd AddPhysicsParameterVals --paramName=kFactor --paramValue="$kfact" --logicalDatasetName="$sample" --physicsGroup=PMG --explanation="$thRef" 
		fi
	    done
	fi
    fi
done
