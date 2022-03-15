#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD building FTAG5 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00364292.physics_Main.merge.AOD.f1002_m2037._lb0163._0006.1 --outputDAODFile art.pool.root --reductionConf FTAG5 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

DAODMerge_tf.py --inputDAOD_FTAG5File DAOD_FTAG5.art.pool.root --outputDAOD_FTAG5_MRGFile art_merged.pool.root

echo "art-result: $? merge"

checkFile.py DAOD_FTAG5.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_FTAG5.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"
