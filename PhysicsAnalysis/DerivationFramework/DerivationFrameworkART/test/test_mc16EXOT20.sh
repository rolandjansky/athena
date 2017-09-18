#!/bin/sh

# art-description: DAOD building EXOT20 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT20 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EXOT20File DAOD_EXOT20.art.pool.root --outputDAOD_EXOT20_MRGFile art_merged.pool.root
