#!/bin/bash
# art-description: Test of converting HepMC file into EVNT one 
# art-include: master/AthGeneration
# art-include: master--HepMC3/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate
## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
cp /cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/HepMC_file/group.phys-gener.dpmjet.420338.gammaA_5TeV_PbPb_mb_photNegEta.TXT.mc16_PART.000.events .
Gen_tf.py --ecmEnergy=5020 --maxEvents=100 \
    --jobConfig=421448  \
    --inputGeneratorFile=group.phys-gener.dpmjet.420338.gammaA_5TeV_PbPb_mb_photNegEta.TXT.mc16_PART.000.events \
    --outputEVNTFile=test_converted.EVNT.pool.root \

echo "art-result: $? generate"


