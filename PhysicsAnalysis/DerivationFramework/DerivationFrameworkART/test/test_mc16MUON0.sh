#!/bin/sh

# art-description: DAOD building MUON0 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf MUON0 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_MUON0File DAOD_MUON0.art.pool.root --outputDAOD_MUON0_MRGFile art_merged.pool.root
