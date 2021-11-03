#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

Reco_tf.py --conditionsTag all:CONDBR2-BLKPA-RUN2-09 --ignoreErrors 'False' --autoConfiguration='everything' --maxEvents '250' --geometryVersion all:ATLAS-R2-2015-04-00-00 --steering='doRAWtoALL' --inputBSFile '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data16_13TeV.00306451.physics_Main.daq.RAW._lb0948._SFO-4._0001.data' --outputDRAW_EGZFile 'myDRAW_EGZ.data' --outputDESDM_IDALIGNFile 'myDESDM_IDALIGN.pool.root' --outputDESDM_SGLELFile 'myDESDM_SGLEL.pool.root' --outputDESDM_SLTTMUFile 'myDESDM_SLTTMU.pool.root' --outputDAOD_IDTRKVALIDFile 'myDAOD_IDTRKVALID.pool.root' --outputDRAW_ZMUMUFile 'myDRAW_ZMUMU.data' --outputAODFile 'myAOD.pool.root' --outputDESDM_EGAMMAFile 'myDESDM_EGAMMA.pool.root' --outputDESDM_MCPFile 'myDESDM_MCP.pool.root' --outputDESDM_CALJETFile 'myDESDM_CALJET.pool.root' --outputDESDM_PHOJETFile 'myDESDM_PHOJET.pool.root' --outputDESDM_TILEMUFile 'myDESDM_TILEMU.pool.root' --outputDRAW_RPVLLFile 'myDRAW_RPVLL.data' --outputDESDM_EXOTHIPFile 'myDESDM_EXOTHIP.pool.root' --outputESDFile 'myESD.pool.root' --outputHISTFile 'myHIST.root' --outputDAOD_IDTIDEFile 'myDAOD_IDTIDE.pool.root' --imf False


rc1=$?
echo "art-result: $rc1 Reco"

rc2=-9999
if [ ${rc1} -eq 0 ]
then
  ArtPackage=$1
  ArtJobName=$2
  art.py compare grid --entries 20 ${ArtPackage} ${ArtJobName} --mode=semi-detailed
  rc2=$?
fi
echo  "art-result: ${rc2} Diff"

