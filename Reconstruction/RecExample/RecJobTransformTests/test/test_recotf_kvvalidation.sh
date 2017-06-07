#!/bin/sh
#
# art-description: Alessandro.de.Salvo@cern.ch, KV validation
# art-type: build

Reco_tf.py '--inputRDOFile' '/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/KitValidation-MC12digitZeePyJT-20.20.8.5.pool.root' '--outputESDFile' 'KitValidation-RecoESDZeeJet-20.20.8.5.pool.root' '--outputAODFile' 'KitValidation-RecoAODZeeJet-20.20.8.5.pool.root' '--maxEvents' '10' '--autoConfiguration' 'everything' '--conditionsTag=OFLCOND-RUN12-SDR-32' '--postInclude' 'KitValidation/kv_reflex.py’


