#!/bin/sh
#
# art-description: RIG convenors - q222 reconstruction
# art-type: build


export TRF_ECHO=True; Reco_tf.py --AMIConfig=q222 --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 'DQMonFlags.doCTPMon=False' 'DQMonFlags.doHLTMon=False' --inputBSFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1'
