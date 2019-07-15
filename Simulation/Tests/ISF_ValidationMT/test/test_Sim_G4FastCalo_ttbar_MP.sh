#!/bin/sh
#
# art-description: MC16-style simulation using the G4FastCalo simulator in AthenaMP
# art-include: 21.0/Athena
# art-include: 21.3/Athena
# art-include: 21.9/Athena
# art-include: master/Athena
# art-type: grid
# art-athena-mt: 4
# art-output: test.HITS.pool.root

# MC16 setup
# ATLAS-R2-2016-01-00-01 and OFLCOND-MC16-SDR-14

Sim_tf.py \
--multiprocess \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'G4FastCalo' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '284500' \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "unordered.HITS.pool.root" \
--maxEvents 250 \
--imf False

rc=$?
echo  "art-result: $rc simulation"
rc2=-9999
if [ $rc -eq 0 ]
then
    tree-orderer.py -i unordered.HITS.pool.root -o test.HITS.pool.root
    rc2=$?
    rm unordered.HITS.pool.root
fi
echo  "art-result: $rc2 reordering"
rc3=-9999
if [ $rc2 -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=summary
    rc3=$?
fi
echo  "art-result: $rc3 regression"
