#!/bin/sh

# art-description: DAOD building SUSY10 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf SUSY10 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_SUSY10File DAOD_SUSY10.art.pool.root --outputDAOD_SUSY10_MRGFile art_merged.pool.root
