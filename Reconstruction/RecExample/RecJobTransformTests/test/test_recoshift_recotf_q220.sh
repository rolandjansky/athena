#!/bin/sh
#
# art-description: RIG convenors - q220 reconstruction
# art-type: build


export TRF_ECHO=True; Reco_tf.py --AMIConfig=q220 --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True' 'DQMonFlags.doCTPMon=False' --inputBSFile='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/data15_cos.00251363.physics_IDCosmic.merge.RAW._lb0057._SFO-ALL._0001.1'
