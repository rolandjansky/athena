#!/bin/sh
#
# art-description: Reco_tf runs on 13TeV collision data 2018 in an isOnline format
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena

#The input file has to be specified twice because Reco_tf requires such an argument, whilst The RecExOnline job options ignore that and instead uses its own fileName to configure
#the input file - without that specified the code crashes out complaining that fileName was not specified.
inputFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data18_13TeV/data18_13TeV.00348885.physics_Main.daq.RAW._lb0827._SFO-8._0002.data"

preIncludeString="RecExOnline/RecExOnline_globalconfig.py,RecExOnline/RecExOnline_recoflags.py,RecExOnline/RecExOnline_monitoring.py"
preExecStringOne="isOnline=True;isOnlineStateless=True;isGlobalMonitoring=False;useEmon=False;useAtlantisEmon=False;evtMax=300;
fileName='${inputFile}'"

Reco_tf.py --inputBSFile="${inputFile}" \
        --preInclude="${preIncludeString}" --preExec="${preExecStringOne}" --postInclude="RecExOnline/RecExOnline_postconfig.py" \
        --AMI=r13286 --outputESDFile myESD.pool.root --outputAODFile myAOD.pool.root --outputHISTFile myHist.root

#Remember retval of transform as art result
RES=$?
echo "art-result: $RES Reco"

