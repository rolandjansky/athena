
#!/bin/sh
#
# art-description: RIG convenors - q221 reconstruction
# art-athena-mt: 4
# art-type: build
# art-include: 21.0/Athena
# art-include: 21.0-TrigMC/Athena
# art-include: master/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena

export TRF_ECHO=True; Reco_tf.py --AMIConfig=q221 --preExec 'rec.doDetailedAuditor=True' 'rec.doNameAuditor=True'

RES=$?

xAODDigest.py myAOD.pool.root digest.txt

echo "art-result: $RES Reco"

