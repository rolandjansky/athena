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
chmod 777 searchExplanation.py

./searchExplanation.py -d "mc15_13TeV.%" -r "%CrossSectionNLOttV%" > file.txt

FILE1=/afs/cern.ch/work/d/deroy/ServiceTaskHandsOn/CentralMCStatusPageMC15New/file.txt
FILE2=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/XsecFiles/list_Xsec_TTbarX_Download.txt
FILE3=/afs/cern.ch/atlas/groups/Generators/CrossSectionInfo/ScriptsCentralPage/InputInformation/CrossSectionReference

cat $FILE1 | while read line
do
    name=`echo $line |awk '{print $1}'`
    sample=`echo $line |awk '{print $3}'`
    kfact=`echo $line |awk '{print $20}'`
    if [ "$name" == "logicalDatasetName" ]; then
	explanation=`echo ${line}|awk '{print $41}'`
	cat $FILE2 | while read line2
	do
	    samplename=`echo $line2 |awk '{print $2}'`
	    if [ "$samplename" == "DSID" ]; then
		continue
	    else
		if [ "$samplename" == "$sample" ]; then
		    highXsec=`echo $line2 |awk '{print $6}'`
		    if [ "$highXsec" == "Undefined" ]; then
			continue
		    fi
		    cat $FILE3 | while read line3
		    do
			[ -z "$line3" ] && continue
			Xsec=`echo $line3 |sed -e 's/\,/ /g' |awk '{print $2}'`
			if [ "$highXsec" == "$Xsec" ]; then
			    thRef=`echo $line3`
			    if [ "$thRef" != "$explanation" ]; then
				echo "two differs !"
		                ami cmd AddPhysicsParameterVals --paramName=kFactor --paramValue="$kfact" --logicalDatasetName="$sample" --physicsGroup=PMG --explanation="$thRef"
			    fi
			fi
		    done
		fi
	    fi
	done
    fi
done	
