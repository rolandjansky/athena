#!/bin/sh

# art-description: DAOD building BPHY3 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf BPHY3 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_BPHY3File DAOD_BPHY3.art.pool.root --outputDAOD_BPHY3_MRGFile art_merged.pool.root
