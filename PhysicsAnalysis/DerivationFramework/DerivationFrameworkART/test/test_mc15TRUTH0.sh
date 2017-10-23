#!/bin/sh

# art-description: DAOD building TRUTH0 mc15
# art-type: grid
# art-output: *.pool.root
# art-output: checkFile.txt

set -e

Reco_tf.py --inputEVNTFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DerivationFrameworkART/EVNT.05192704._020091.pool.root.1 --outputDAODFile art.pool.root --reductionConf TRUTH0 --maxEvents 1000

DAODMerge_tf.py --maxEvents 5 --inputDAOD_TRUTH0File DAOD_TRUTH0.art.pool.root --outputDAOD_TRUTH0_MRGFile art_merged.pool.root --autoConfiguration ProjectName RealOrSim BeamType ConditionsTag DoTruth InputType BeamEnergy LumiFlags TriggerStream --athenaopts="-s" 
checkFile.py DAOD_TRUTH0.art.pool.root > checkFile.txt