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
logfilename=`echo $1` 
FILE0="$logfilename"
FILE1=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/InputInformation/CrossSectionReference
cat $FILE0 | while read line
do 
    sample=`echo $line |awk '{print $1}'`
    kfactVar=`echo $line |awk '{print $2}'`
    AMIkfact=`echo $line |awk '{print $6}'`
    PMGkfact=`echo $line |awk '{print $4}'`
    PMGhighXsec=`echo $line |awk '{print $8}'`
    if [[ "$kfactVar" == "Kfactor" && "$AMIkfact" == "1.0" ]]; then
	cat $FILE1 | while read line1
	do
	    highXsec=`echo $line1 |sed -e 's/\,/ /g' |awk '{print $2}'`
	    if [ "highXsec" == "Undefined" ]; then
		continue
	    fi
	    if [ "$highXsec" == "$PMGhighXsec" ]; then
		echo "highXsec" ${highXsec}
		echo "PMGhighXsec" ${PMGhighXsec}
		thRef=`echo $line1`
		if [ "$thRef" ]; then 
		    echo "satisfied"
	            ami cmd AddPhysicsParameterVals --paramName=kFactor --paramValue="$PMGkfact" --logicalDatasetName="$sample" --physicsGroup=PMG --explanation="$thRef"
		else
		    ami cmd AddPhysicsParameterVals --paramName=kFactor --paramValue="$PMGkfact" --logicalDatasetName="$sample" --physicsGroup=PMG --explanation="unknown"
		fi
	    fi
	done
    fi
done

