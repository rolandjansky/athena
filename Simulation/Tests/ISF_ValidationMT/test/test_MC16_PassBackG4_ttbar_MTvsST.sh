#!/bin/sh
#
# art-description: Run MT and ST simulation inside ISF with G4 passback, reading ttbar events, writing HITS, using MC16 geometry and conditions
# art-include: master/Athena

# art-type: grid
# art-athena-mt: 8
# art-output: log.*
# art-output: test.MT.HITS.pool.root
# art-output: test.ST.HITS.pool.root

export ATHENA_CORE_NUMBER=8

Sim_tf.py \
--multithreaded \
--inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
--outputHITSFile "test.MT.HITS.pool.root" \
--maxEvents 10 \
--geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
--conditionsTag 'default:OFLCOND-MC16-SDR-14' \
--DataRunNumber '284500' \
--physicsList 'FTFP_BERT_ATL' \
--truthStrategy 'MC15aPlus' \
--simulator 'PassBackG4MT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
--imf False

rc=$?
mv log.EVNTtoHITS log.PassBackG4MTAthenaMT
echo  "art-result: $rc PassBackG4MTAthenaMT"
rc2=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    unset ATHENA_CORE_NUMBER
    Sim_tf.py \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
  --outputHITSFile "test.ST.HITS.pool.root" \
  --maxEvents 10 \
  --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
  --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
  --DataRunNumber '284500' \
  --physicsList 'FTFP_BERT_ATL' \
  --truthStrategy 'MC15aPlus' \
  --simulator 'PassBackG4MT' \
  --postInclude 'default:PyJobTransforms/UseFrontier.py' \
  --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
  --preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
  --imf False
    rc2=$?
    mv log.EVNTtoHITS log.PassBackG4MTAthena
fi
echo  "art-result: $rc2 PassBackG4MTAthena"
rc3=-9999
if [ $rc -eq 0 ]
then
    ArtPackage=$1
    ArtJobName=$2
    unset ATHENA_CORE_NUMBER
    Sim_tf.py \
    --inputEVNTFile "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/SimCoreTests/valid1.410000.PowhegPythiaEvtGen_P2012_ttbar_hdamp172p5_nonallhad.evgen.EVNT.e4993.EVNT.08166201._000012.pool.root.1" \
  --outputHITSFile "test.ST.old.HITS.pool.root" \
  --maxEvents 10 \
  --geometryVersion 'default:ATLAS-R2-2016-01-00-01' \
  --conditionsTag 'default:OFLCOND-MC16-SDR-14' \
  --DataRunNumber '284500' \
  --physicsList 'FTFP_BERT_ATL' \
  --truthStrategy 'MC15aPlus' \
  --simulator 'PassBackG4' \
  --postInclude 'default:PyJobTransforms/UseFrontier.py' \
  --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
  --preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' \
  --postExec 'svcMgr.ISF_ParticleBrokerSvcNoOrdering.ParticleOrderingTool="ISF__EnergyParticleOrderingTool";svcMgr.ISF_ParticleBrokerSvcNoOrdering.AlwaysUseGeoIDSvc=True' \
  --imf False
    rc3=$?
    mv log.EVNTtoHITS log.PassBackG4Athena
fi
echo  "art-result: $rc3 PassBackG4Athena"
rc4=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.MT.HITS.pool.root test.ST.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 PassBackG4MT_STvsMT"
rc5=-9999
if [ $rc3 -eq 0 ]
then
    acmd.py diff-root test.MT.HITS.pool.root test.ST.old.HITS.pool.root --error-mode resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc5=$?
fi
echo  "art-result: $rc5 PassBackG4MTvsPassBackG4"
