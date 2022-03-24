#!/bin/bash

# art-description: DirectIOART Athena Reco_tf.py inputFile:RDO protocol=DAVS
# art-type: grid
# art-output: *.pool.root
# art-include: master/Athena

set -e

Reco_tf.py --AMI q221 --conditionsTag "default:OFLCOND-MC16-SDR-RUN2-09" "RDOtoRDOTrigger:OFLCOND-MC16-SDR-RUN2-08-02" --athenaopts='--threads=1' --inputRDOFile davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc16_13TeV/90/96/RDO.11373415._000001.pool.root.1 --outputRDO_TRIGFile art.pool.root --preExec 'from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.AODEDMSet="AODFULL"'

echo "art-result: $? DirectIOART_Athena_RecoTF_inputRDO_protocol_DAVS"
