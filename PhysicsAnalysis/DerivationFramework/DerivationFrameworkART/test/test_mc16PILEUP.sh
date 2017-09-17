#!/bin/sh

# art-description: NTUP building PILEUP mc16
# art-type: grid
# art-output: *.root

Reco_tf.py --inputAODFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/AOD.11866988._000378.pool.root.1 --outputNTUP_PILEUPFile art.ntup.root --maxEvents 5000
