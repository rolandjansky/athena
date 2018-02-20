#!/bin/sh
#
# art-description: RIG convenors - q223 reconstruction
# art-type: build

export TRF_ECHO=True; Reco_tf.py --AMIConfig=q223 --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 'DQMonFlags.doCTPMon=False' --inputBSFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data15_comm.00264034.physics_MinBias.daq.RAW._lb0644._SFO-6._0001.data'


