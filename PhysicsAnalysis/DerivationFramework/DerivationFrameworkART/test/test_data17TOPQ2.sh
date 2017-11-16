#!/bin/sh

# art-description: DAOD building TOPQ2 data17
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00327342.physics_Main.merge.AOD.f838_m1824._lb0300._0001.1 --outputDAODFile art.pool.root --reductionConf TOPQ2 --maxEvents -1 --preExec 'rec.doApplyAODFix.set_Value_and_Lock(True);from BTagging.BTaggingFlags import BTaggingFlags;BTaggingFlags.CalibrationTag = "BTagCalibRUN12Onl-08-40" ' 

DAODMerge_tf.py --inputDAOD_TOPQ2File DAOD_TOPQ2.art.pool.root --outputDAOD_TOPQ2_MRGFile art_merged.pool.root
checkFile.py DAOD_TOPQ2.art.pool.root > checkFile.txt