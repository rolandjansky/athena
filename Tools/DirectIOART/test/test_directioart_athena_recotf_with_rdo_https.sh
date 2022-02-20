#!/bin/bash

# art-description: DirectIOART Athena Reco_tf.py inputFile:RDO protocol=HTTPS
# art-type: grid
# art-output: *.pool.root
# art-include: master/Athena

set -e

Reco_tf.py --AMI q221 --conditionsTag "default:OFLCOND-MC16-SDR-RUN2-09" "RDOtoRDOTrigger:OFLCOND-MC16-SDR-RUN2-08-02" --athenaopts='--threads=1' --inputRDOFile https://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc16_13TeV/90/96/RDO.11373415._000001.pool.root.1 --outputRDO_TRIGFile art.pool.root

echo "art-result: $? DirectIOART_Athena_RecoTF_inputRDO_protocol_HTTPS"
