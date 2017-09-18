#!/bin/sh

# art-description: DAOD building EXOT15 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT15 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EXOT15File DAOD_EXOT15.art.pool.root --outputDAOD_EXOT15_MRGFile art_merged.pool.root
