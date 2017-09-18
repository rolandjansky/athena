#!/bin/sh

# art-description: DAOD building EXOT8 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT8 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EXOT8File DAOD_EXOT8.art.pool.root --outputDAOD_EXOT8_MRGFile art_merged.pool.root
