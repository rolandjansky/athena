#!/bin/sh

# art-description: DAOD building EGAM7 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EGAM7 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EGAM7File DAOD_EGAM7.art.pool.root --outputDAOD_EGAM7_MRGFile art_merged.pool.root
