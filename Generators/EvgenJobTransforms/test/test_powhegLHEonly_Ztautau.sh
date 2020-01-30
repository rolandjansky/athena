#!/bin/bash
# art-description: Generation test Powheg LHE only Z->tautau
# art-type: build
# art-include: 21.6/AthGeneration
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
rm *;
Generate_tf_txt.py --ecmEnergy=13000 --runNumber=369108 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID369xxx/MC15.369108.Powheg_AZNLOCTEQ6L1_Ztautau.py \
    --outputTXTFile=test_powheg_Ztautau.TXT \

echo "art-result: $? generate"
rm -fr _joproxy15


