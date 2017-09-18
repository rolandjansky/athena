#!/bin/sh

# art-description: DAOD building EGAM5 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EGAM5 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EGAM5File DAOD_EGAM5.art.pool.root --outputDAOD_EGAM5_MRGFile art_merged.pool.root
