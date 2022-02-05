#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-athena-mt: 8

Reco_tf.py \
--athenaopts "RDOtoRDOTrigger:--threads=8" "RAWtoESD:--threads=8" "ESDtoAOD:--threads=8" \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--conditionsTag 'default:OFLCOND-MC16-SDR-RUN2-09' \
--valid 'True' \
--pileupFinalBunch '6' \
--numberOfHighPtMinBias '0.2595392' \
--autoConfiguration 'everything' \
--numberOfLowPtMinBias '99.2404608' \
--steering 'doRDO_TRIG' \
--preInclude 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configLumi_run310000.py' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--postExec 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]' "ESDtoAOD:fixedAttrib=[s if \"CONTAINER_SPLITLEVEL = '99'\" not in s else \"\" for s in svcMgr.AthenaPoolCnvSvc.PoolAttributes];svcMgr.AthenaPoolCnvSvc.PoolAttributes=fixedAttrib" "RDOtoRDOTrigger:conddb.addOverride(\"/CALO/Ofl/Noise/PileUpNoiseLumi\",\"CALOOflNoisePileUpNoiseLumi-mc15-mu30-dt25ns\")" 'ESDtoAOD:CILMergeAOD.removeItem("xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.LATERAL.LONGITUDINAL.SECOND_R.SECOND_LAMBDA.CENTER_MAG.CENTER_LAMBDA.FIRST_ENG_DENS.ENG_FRAC_MAX.ISOLATION.ENG_BAD_CELLS.N_BAD_CELLS.BADLARQ_FRAC.ENG_BAD_HV_CELLS.N_BAD_HV_CELLS.ENG_POS.SIGNIFICANCE.CELL_SIGNIFICANCE.CELL_SIG_SAMPLING.AVG_LAR_Q.AVG_TILE_Q.EM_PROBABILITY.PTD.BadChannelList");CILMergeAOD.add("xAOD::CaloClusterAuxContainer#CaloCalTopoClustersAux.N_BAD_CELLS.ENG_BAD_CELLS.BADLARQ_FRAC.AVG_TILE_Q.AVG_LAR_Q.CENTER_MAG.ENG_POS.CENTER_LAMBDA.SECOND_LAMBDA.SECOND_R.ISOLATION.EM_PROBABILITY");StreamAOD.ItemList=CILMergeAOD()' \
--preExec 'all:rec.Commissioning.set_Value_and_Lock(True);from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False)' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--numberOfCavernBkg '0' \
--inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/\* \
--maxEvents=100 \
--runNumber=410000 \
--jobNumber=1 \
--inputLowPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/\* \
--inputHighPtMinbiasHitsFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/\* \
--outputAODFile=myAOD.pool.root --outputRDOFile=myRDO.pool.root --outputESDFile=myESD.pool.root --imf False

rc1=$?
echo "art-result: $rc1 Reco"

# Workaround for the problem "Looping job killed by pilot trans". A message is printed every 30 minutes to avoid killing the job by the pilot. This was suggested in the email thread with atlas-adc-dpa@cern.ch by Serhan on 23.4.2021.

# Periodically print elapsed time while the long command executes
file=$(mktemp)

progress() {
  pc=0;
  inc=1800; # print elapsed time every 1800 seconds
  while [ -e ${file} ];
    do
      echo "[WRAPPER] Elapsed time: $(expr ${pc} \* ${inc}) seconds..."
      sleep ${inc}
      ((pc++))
    done
}

progress &

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  echo "Running art.py command"
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --ignore-exit-code diff-pool --ignore-leave 'Token' --ignore-leave 'index_ref' --ignore-leave '(.*)_timings\.(.*)' --ignore-leave '(.*)_mems\.(.*)' --ignore-leave '(.*)TrigCostContainer(.*)' --ignore-leave '(.*)HLTNav_Summary_OnlineSlimmed(.*)'  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

# Remove tmp file after done
rm -f $file

