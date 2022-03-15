#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building JETM5 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00339070.physics_ZeroBias.merge.AOD.f887_m1892._lb0998-lb1007._0001.1 --outputDAODFile art.pool.root --reductionConf JETM5 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_JETM5File DAOD_JETM5.art.pool.root --outputDAOD_JETM5_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_JETM5.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_JETM5.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
