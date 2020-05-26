#!/bin/sh
#
# art-description: Run MT and ST simulation outside ISF, reading ttbar events, writing HITS, using MC16 geometry and conditions

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
--simulator 'G4FastCaloMT' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' \
--preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
--preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' #\
#--postExec 'EVNTtoHITS:ServiceMgr.MessageSvc.enableSuppression=False;topSeq.ISF_Kernel_G4FastCaloMT.OutputLevel=VERBOSE;' \
#--imf False

rc=$?
mv log.EVNTtoHITS log.G4FastCaloMTAthenaMT
echo  "art-result: $rc G4FastCaloMTAthenaMT"
rc=0
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
  --simulator 'G4FastCaloMT' \
  --postInclude 'default:PyJobTransforms/UseFrontier.py' \
  --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
  --preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' #\
#  --postExec 'EVNTtoHITS:ServiceMgr.MessageSvc.enableSuppression=False;topSeq.ISF_Kernel_G4FastCaloMT.OutputLevel=VERBOSE;' \
#  --imf False
  rc2=$?
  mv log.EVNTtoHITS log.G4FastCaloMTAthena
fi
echo  "art-result: $rc2 G4FastCaloMTAthena"
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
  --simulator 'G4FastCalo' \
  --postInclude 'default:PyJobTransforms/UseFrontier.py' \
  --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' \
  --preExec 'EVNTtoHITS:simFlags.TightMuonStepping=True' #\
#  --postExec 'EVNTtoHITS:ServiceMgr.MessageSvc.enableSuppression=False;topSeq.ISF_Kernel_G4FastCalo.OutputLevel=VERBOSE;ServiceMgr.ISF_AFIIParticleBrokerSvc.OutputLevel=VERBOSE;' \
#  --imf False
  rc3=$?
  mv log.EVNTtoHITS log.G4FastCaloAthena
fi
echo  "art-result: $rc3 G4FastCaloAthena"
rc4=-9999
if [ $rc2 -eq 0 ]
then
    acmd.py diff-root test.MT.HITS.pool.root test.ST.HITS.pool.root --error-mode=resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc4=$?
fi
echo  "art-result: $rc4 G4FastCaloMT_STvsMT"
rc5=-9999
if [ $rc3 -eq 0 ]
then
    acmd.py diff-root test.ST.HITS.pool.root test.ST.old.HITS.pool.root --error-mode=resilient --mode=semi-detailed --order-trees --ignore-leaves RecoTimingObj_p1_EVNTtoHITS_timings index_ref
    rc5=$?
fi
echo  "art-result: $rc5 G4FastCaloMTvsG4FastCalo"
