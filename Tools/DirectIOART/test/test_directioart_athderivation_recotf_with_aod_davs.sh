#!/bin/bash

# art-description: DirectIOART AthDerivation Reco_tf.py inputFile:RDO protocol=DAVS reductionConf:TEST1
# art-type: grid
# art-output: *.pool.root
# art-include: 21.2/AthDerivation

set -e

Reco_tf.py --inputAODFile davs://lcg-lrz-http.grid.lrz.de:443/pnfs/lrz-muenchen.de/data/atlas/dq2/atlasdatadisk/rucio/mc15_13TeV/ed/68/AOD.05536542._000001.pool.root.1 --outputDAODFile art.pool.root --reductionConf TEST1 --maxEvents 100

echo "art-result: $? DirectIOART_AthDerivation_RecoTF_inputAOD_protocol_DAVS_TEST1"