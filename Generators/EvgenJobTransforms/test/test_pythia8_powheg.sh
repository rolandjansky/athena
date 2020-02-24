#!/bin/bash
# art-description: Generation test Pythia8 Powheg 
# art-include: master/Athena
# art-type: build
# art-output: *.root
# art-output: log.generate

## Any arguments are considered overrides, and will be added at the end
export TRF_ECHO=True;
Generate_tf.py --ecmEnergy=8000 --runNumber=429695 --maxEvents=10 \
    --jobConfig=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest/share/DSID429xxx/MC15.429695.PowhegPythia8EvtGen_A14NNPDF23LO_Wplusenu.py \
    --inputGeneratorFile=/afs/cern.ch/atlas/offline/ProdData/17.2.X/group.phys-gener.Powheg_CT10.147800.Wplusenu_8TeV.TXT.mc12_v2._00001.tar.gz \
    --outputEVNTFile=test_powhegpythia8_Wpluswnu.EVNT.pool.root \

echo "art-result: $? generate"
rm -fr _joproxy15

    
