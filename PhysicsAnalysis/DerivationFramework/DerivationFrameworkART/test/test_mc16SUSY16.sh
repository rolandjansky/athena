#!/bin/sh

# art-description: DAOD building SUSY16 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf SUSY16 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_SUSY16File DAOD_SUSY16.art.pool.root --outputDAOD_SUSY16_MRGFile art_merged.pool.root
