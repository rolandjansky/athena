#!/bin/sh
#
# art-description: David Rousseau
# art-type: build

export TRF_ECHO=True; Reco_tf.py --inputRDOFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/RDO.pool.root --maxEvents 5 --autoConfiguration everything --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 'rec.doCalo=False' 'rec.doInDet=False' 'rec.doMuon=False' 'rec.doJetMissingETTag=False' 'rec.doEgamma=False' 'rec.doMuonCombined=False' 'rec.doTau=False' 'rec.doTrigger=False' --outputESDFile my.ESD.pool.root

