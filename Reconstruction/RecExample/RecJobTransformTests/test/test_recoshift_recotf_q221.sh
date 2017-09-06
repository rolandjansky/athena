#!/bin/sh
#
# art-description: RIG convenors - q221 reconstruction
# art-type: build

export TRF_ECHO=True; Reco_tf.py --AMIConfig=q221 --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True'
