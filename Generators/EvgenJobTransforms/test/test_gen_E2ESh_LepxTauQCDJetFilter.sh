#!/bin/bash
# art-description: EVNT2EVNT filtering - Sherpa files - Lepton+xAODTau+QCDTruthJetFilters
# art-type: build
# art-include: master/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Gen_tf.py --ecmEnergy=13000 --jobConfig=421408 --maxEvents=10 \
    --inputEVNT_PreFile=/eos/atlas/atlascerngroupdisk/data-art/grid-input/Evgen_E2E/mc15_13TeV/EVNT.25508216._000003.pool.root.1,/eos/atlas/atlascerngroupdisk/data-art/grid-input/Evgen_E2E/mc15_13TeV/EVNT.25508216._000001.pool.root.1 \
    --outputEVNTFile=test_E2E_SH_LeptonxTaunotQCDTruthJetFilters.EVNT.pool.root \

echo "art-result: $? generate"


