#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-athena-mt: 8


# Test written on 16.12.2021 based on a recent Run2 Reprocessing MC task: https://bigpanda.cern.ch/task/27175309/
# Updated inputs on 30.12.2021

HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3681/HITS.25836812._004813.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20_13TeV.900149.PG_single_nu_Pt50.digit.RDO.e8307_s3482_s3136_d1715/RDO.26811908._031801.pool.root.1"


export ATHENA_PROC_NUMBER=8
export ATHENA_CORE_NUMBER=8
Reco_tf.py \
--inputHITSFile ${HITS_File} \
--inputRDO_BKGFile ${RDO_BKG_File} \
--asetup="RDOtoRDOTrigger:Athena,21.0,latest" \
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
--runNumber="410470" \
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
  art.py compare ref . /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TCT_Run2Reprocessing_references_for_comparison/test_mc20e_2021-12-30T2101 --entries 30 --mode=semi-detailed --order-trees --ignore-exit-code diff-pool
  rc3=$?
fi
echo  "art-result: ${rc3} Comparison with fixed reference"


