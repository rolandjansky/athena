#!/bin/sh
#
# art-description: Run FastChain with MC+MC Overlay for MC16a, ttbar
# art-type: grid
# art-include: master/Athena

# art-output: *.root                                                           
# art-output: config.txt
# art-output: RAWtoESD_config.txt

events=25
HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.simul.HITS.e4993_s3091/HITS.10504490._000425.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayMonitoringRTT/PileupPremixing/22.0/v4/RDO.merged-pileup-MT.100events.pool.root"
RDO_File="MC_plus_MC.RDO.pool.root"
AOD_File="MC_plus_MC.AOD.pool.root"
NTUP_File="MC_plus_MC.NTUP.pool.root"

FastChain_tf.py \
  --inputHITSFile ${HITS_File} \
  --inputRDO_BKGFile ${RDO_BKG_File} \
  --outputRDOFile ${RDO_File} \
  --maxEvents ${events} \
  --skipEvents 0 \
  --digiSeedOffset1 511 \
  --digiSeedOffset2 727 \
  --conditionsTag OFLCOND-MC16-SDR-RUN2-09 \
  --geometryVersion ATLAS-R2-2016-01-00-01 \
  --preExec 'from LArROD.LArRODFlags import larRODFlags;larRODFlags.NumberOfCollisions.set_Value_and_Lock(20);larRODFlags.nSamples.set_Value_and_Lock(4);larRODFlags.doOFCPileupOptimization.set_Value_and_Lock(True);larRODFlags.firstSample.set_Value_and_Lock(0);larRODFlags.useHighestGainAutoCorr.set_Value_and_Lock(True); from LArDigitization.LArDigitizationFlags import jobproperties;jobproperties.LArDigitizationFlags.useEmecIwHighGain.set_Value_and_Lock(False);' \
  --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' 'all:CfgMgr.MessageSvc().setError+=["HepMcParticleLink"]'\
  --imf False


rc=$?
echo "art-result: ${rc} HITStoRDO"


rc1=999
rc2=999
if [ ${rc} -eq 0 ]
then
    # Reconstruction
    Reco_tf.py --inputRDOFile ${RDO_File} --maxEvents '-1' \
               --autoConfiguration=everything \
               --outputAODFile ${AOD_File} \
               --steering 'doRDO_TRIG' \
               --athenaopts "all:--threads=1" \
               --postExec 'RAWtoESD:from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("RAWtoESD_config.txt")' \
               --imf False

     rc2=$?
     if [ ${rc2} -eq 0 ]
     then
         # NTUP prod.
         Reco_tf.py --inputAODFile ${AOD_File} --maxEvents '-1' \
                    --outputNTUP_PHYSVALFile ${NTUP_File} \
                    --ignoreErrors True \
                    --validationFlags 'doInDet' \
                    --valid 'True'
         rc3=$?
     fi
fi


echo  "art-result: ${rc2} RDOtoAOD"
echo  "art-result: ${rc3} AODtoNTUP"
