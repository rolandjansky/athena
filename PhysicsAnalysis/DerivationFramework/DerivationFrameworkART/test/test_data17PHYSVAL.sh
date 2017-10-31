#!/bin/sh

# art-description: DAOD building PHYSVAL data17
# art-type: grid
# art-output: *.pool.root

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00327342.physics_Main.merge.AOD.f838_m1824._lb0300._0001.1 --outputDAODFile art.pool.root --reductionConf PHYSVAL --maxEvents 5000 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" ' 

DAODMerge_tf.py --maxEvents 5 --inputDAOD_PHYSVALFile DAOD_PHYSVAL.art.pool.root --outputDAOD_PHYSVAL_MRGFile art_merged.pool.root

Reco_tf.py --inputAODFile DAOD_PHYSVAL.art.pool.root --preExec "all:from InDetPhysValMonitoring.InDetPhysValJobProperties import InDetPhysValFlags; InDetPhysValFlags.doValidateTightPrimaryTracks.set_Value_and_Lock(True);" --valid="True" --validationFlags doExample,doInDet,doMuon,doTopoCluster,doPFlow,doJet,doBtag,doMET,doEgamma,doTau --outputNTUP_PHYSVALFile NTUP_PHYSVAL.art.root
