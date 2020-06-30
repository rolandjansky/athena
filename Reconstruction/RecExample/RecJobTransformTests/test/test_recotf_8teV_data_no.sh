#!/bin/sh
#
# art-description: David Rousseau, data12_8TeV reconstruction, all reco switched off
# art-athena-mt: 4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena


export TRF_ECHO=True; Reco_tf.py --inputBSFile /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1 --maxEvents 5 --autoConfiguration everything --preExec 'rec.doCalo=False' 'rec.doInDet=False' 'rec.doMuon=False' 'rec.doJetMissingETTag=False' 'rec.doEgamma=False' 'rec.doMuonCombined=False' 'rec.doTau=False' 'rec.doTrigger=False' 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' --outputESDFile my.ESD.pool.root

RES=$?
echo "art-result: $RES Reco"


