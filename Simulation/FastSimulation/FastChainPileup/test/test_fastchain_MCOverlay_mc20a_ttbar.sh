#!/bin/sh

# art-description: Run FastChain with MC+MC Overlay without reco for MC20a, ttbar
# art-type: grid
# art-include: master/Athena

# art-output: *.root
# art-output: config.txt
# art-output: RAWtoESD_config.txt

events=25
HITS_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.410470.PhPy8EG_A14_ttbar_hdamp258p75_nonallhad.simul.HITS.e6337_s3681/HITS.25836812._004813.pool.root.1"
RDO_BKG_File="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/OverlayTests/PresampledPileUp/22.0/Run2/large/mc20_13TeV.900149.PG_single_nu_Pt50.digit.RDO.e8307_s3482_s3136_d1713/RDO.26811885._035498.pool.root.1"
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
  --conditionsTag OFLCOND-MC16-SDR-RUN2-09  \
  --geometryVersion ATLAS-R2-2016-01-00-01 \
  --preInclude 'all:Campaigns/MC20a.py' \
  --postExec 'from AthenaCommon.ConfigurationShelve import saveToAscii;saveToAscii("config.txt")' \
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
