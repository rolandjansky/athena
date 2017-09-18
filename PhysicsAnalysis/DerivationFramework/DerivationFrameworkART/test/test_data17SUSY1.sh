#!/bin/sh

# art-description: DAOD building SUSY1 data17
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00327342.physics_Main.merge.AOD.f838_m1824._lb0300._0001.1 --outputDAODFile art.pool.root --reductionConf SUSY1 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_SUSY1File DAOD_SUSY1.art.pool.root --outputDAOD_SUSY1_MRGFile art_merged.pool.root
