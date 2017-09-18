#!/bin/sh

# art-description: DAOD building EXOT0 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf EXOT0 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_EXOT0File DAOD_EXOT0.art.pool.root --outputDAOD_EXOT0_MRGFile art_merged.pool.root
