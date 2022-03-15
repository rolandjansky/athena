#!/bin/sh

# art-include: 21.2/AthDerivation
# art-description: DAOD compare to ref FTAG1 data18
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data18_13TeV.00348403.physics_Main.merge.AOD.f920_m1947._lb0829._0001.1 --outputDAODFile art.pool.root --reductionConf FTAG1 --maxEvents 500 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True); from AthenaCommon.AlgSequence import AlgSequence; topSequence = AlgSequence(); topSequence += CfgMgr.xAODMaker__DynVarFixerAlg( "InDetTrackParticlesFixer", Containers = [ "InDetTrackParticlesAux." ] )' 

echo "art-result: $? reco"

cp DAOD_FTAG1.art.pool.root DAOD.pool.root

art-diff.py ./DAOD.pool.root /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/ReferenceFiles/21.2.34.0/data18_13TeV.00349533.f929_m1955._lb0352/DAOD_ftag1.data18.pool.root --diff-type=diff-pool 

echo  "art-result: $? diff-pool"

rm -f DAOD.pool.root
