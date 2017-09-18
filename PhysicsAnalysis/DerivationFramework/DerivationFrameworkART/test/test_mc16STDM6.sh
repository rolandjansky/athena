#!/bin/sh

# art-description: DAOD building STDM6 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf STDM6 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_STDM6File DAOD_STDM6.art.pool.root --outputDAOD_STDM6_MRGFile art_merged.pool.root
