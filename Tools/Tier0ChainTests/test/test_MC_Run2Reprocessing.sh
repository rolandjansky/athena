#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8


# Test written on 16.12.2021 based on a recent Run2 Reprocessing MC task: https://bigpanda.cern.ch/task/27175309/

export ATHENA_PROC_NUMBER=8
export ATHENA_CORE_NUMBER=8
Reco_tf.py \
--inputHITSFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TCT_MC_Run2Reprocessing/HITS.25836923._001783.pool.root.1" \
--asetup="RDOtoRDOTrigger:Athena,21.0.131" \
--maxEvents="200" \
--multithreaded="True" \
--postInclude "default:PyJobTransforms/UseFrontier.py" "all:PyJobTransforms/DisableFileSizeLimit.py" \
--preInclude "all:Campaigns/MC20e.py" \
--skipEvents="0" \
--autoConfiguration="everything" \
--conditionsTag "default:OFLCOND-MC16-SDR-RUN2-09" "RDOtoRDOTrigger:OFLCOND-MC16-SDR-RUN2-08-02" \
--geometryVersion="default:ATLAS-R2-2016-01-00-01" \
--digiSeedOffset1="476" \
--digiSeedOffset2="476" \
--inputRDO_BKGFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TCT_MC_Run2Reprocessing/RDO.26835823._011177.pool.root.1" \
--AMITag="r13145" \
--steering "doOverlay" "doRDO_TRIG" \
--outputAODFile="AOD.27175309._011910.pool.root.1" \
--triggerConfig="RDOtoRDOTrigger=MCRECO:DBF:TRIGGERDBMC:2233,87,314" \
--outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root \
--imf False


rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 30 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
  rc2=$?
fi
echo  "art-result: ${rc2} Comparison with the latest result"


rc3=-9999
if [ ${rc1} -eq 0 ]
then
  # Need to update the reference
#art.py compare ref . /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TrfTestsART_22.0-mc20/test_trf_data15_mt_2021-12-04T2101 --entries 30 --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
  rc3=$?
fi
echo  "art-result: ${rc3} Comparison with fixed reference"


