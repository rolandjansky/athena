#!/bin/sh

# art-description: DAOD building HIGG1D2 data17
# art-type: grid
# art-output: *.pool.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/data17_13TeV.00327342.physics_Main.merge.AOD.f838_m1824._lb0300._0001.1 --outputDAODFile art.pool.root --reductionConf HIGG1D2 --maxEvents 5000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_HIGG1D2File DAOD_HIGG1D2.art.pool.root --outputDAOD_HIGG1D2_MRGFile art_merged.pool.root
