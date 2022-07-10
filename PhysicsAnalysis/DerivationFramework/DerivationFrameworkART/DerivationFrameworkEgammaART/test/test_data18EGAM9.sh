#!/bin/sh

# art-include: master/AthDerivation
# art-include: master/Athena
# art-description: DAOD building EGAM9 data18
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt
# art-output: checkxAOD.txt
# art-output: checkIndexRefs.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data18_13TeV.00357772.physics_Main.recon.AOD.r13286/AOD.27654050._000557.pool.root.1 --outputDAODFile art.pool.root --reductionConf EGAM9 --maxEvents -1 --preExec 'from AthenaCommon.DetFlags import DetFlags; DetFlags.detdescr.all_setOff(); DetFlags.BField_setOn(); DetFlags.digitize.all_setOff(); DetFlags.detdescr.Calo_setOn(); DetFlags.simulate.all_setOff(); DetFlags.pileup.all_setOff(); DetFlags.overlay.all_setOff(); DetFlags.detdescr.Muon_setOn();' --postExec 'from DerivationFrameworkJetEtMiss.JetCommon import swapAlgsInSequence; swapAlgsInSequence(topSequence,"jetalg_ConstitModCorrectPFOCSSKCHS_GPFlowCSSK", "UFOInfoAlgCSSK" );'

echo "art-result: $? reco"

checkFile.py DAOD_EGAM9.art.pool.root > checkFile.txt

echo "art-result: $?  checkfile"

checkxAOD.py DAOD_EGAM9.art.pool.root > checkxAOD.txt

echo "art-result: $?  checkxAOD"

checkIndexRefs.py DAOD_EGAM9.art.pool.root > checkIndexRefs.txt

echo "art-result: $?  checkIndexRefs"
