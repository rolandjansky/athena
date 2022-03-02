#!/bin/sh
#
# art-description: MC21-style simulation using ATLFAST3MT in AthenaMP
# art-include: master/Athena
# art-type: grid
# art-athena-mt: 4
# art-output: test.HITS.pool.root

# RUN3 setup
# ATLAS-R3S-2021-01-00-02 and OFLCOND-MC16-SDR-RUN3-01
# Increase maxEvents to 250 once ATLASSIM-5576 is fixed.

export ATHENA_CORE_NUMBER=4

Sim_tf.py \
--multiprocess \
--conditionsTag 'default:OFLCOND-MC16-SDR-RUN3-01' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'ATLFAST3MT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--DataRunNumber '330000' \
--geometryVersion 'default:ATLAS-R3S-2021-01-00-02_VALIDATION' \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.HITS.pool.root" \
--maxEvents 12 \
--imf False

rc=$?
rc2=-9999
echo  "art-result: $rc simOLD"
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    art.py compare grid --entries 10 ${ArtPackage} ${ArtJobName} --mode=semi-detailed --order-trees --diff-root
    rc2=$?
fi

echo  "art-result: $rc2 regression"
