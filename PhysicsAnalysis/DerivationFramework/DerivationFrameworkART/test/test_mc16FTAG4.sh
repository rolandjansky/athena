#!/bin/sh

# art-description: DAOD building FTAG4 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf FTAG4 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_FTAG4File DAOD_FTAG4.art.pool.root --outputDAOD_FTAG4_MRGFile art_merged.pool.root
