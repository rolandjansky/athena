#!/bin/sh

# art-description: DAOD building MUON3 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf MUON3 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_MUON3File DAOD_MUON3.art.pool.root --outputDAOD_MUON3_MRGFile art_merged.pool.root
