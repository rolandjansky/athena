#!/bin/sh

# art-description: DAOD building EGAM9 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EGAM9 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EGAM9File DAOD_EGAM9.art.pool.root --outputDAOD_EGAM9_MRGFile art_merged.pool.root
