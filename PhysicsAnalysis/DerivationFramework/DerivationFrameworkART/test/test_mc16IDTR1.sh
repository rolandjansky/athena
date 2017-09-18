#!/bin/sh

# art-description: DAOD building IDTR1 mc16
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputDAODFile art.pool.root --reductionConf IDTR1 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_IDTR1File DAOD_IDTR1.art.pool.root --outputDAOD_IDTR1_MRGFile art_merged.pool.root
