#!/bin/bash
# MC16e-like flat pile-up between 50 and 60
#
# NOTES: VTune needs to be installed on the local machine and setup
# before asetup is called. To be able to run this example
# 'Control/PerformanceMonitoring/PerfMonVTune' needs to be built.
#

DigiOutFileName="mc16e_premixing_MT.RDO.pool.root"

InputHitsFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/DigitizationTests/mc16_valid.422075.ParticleGun_single_nu_Pt50.simul.HITS.e6739_e5984_s3126.HITS.14436922._003949.pool.root"
HighPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361239.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_high.merge.HITS.e4981_s3087_s3089/*"
LowPtMinbiasHitsFiles="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc16_13TeV.361238.Pythia8EvtGen_A3NNPDF23LO_minbias_inelastic_low.merge.HITS.e4981_s3087_s3089/*"


vtune -mrte-mode=native -start-paused -run-pass-thru=--no-altstack -collect hotspots -- \
Digi_tf.py \
--PileUpPresampling True \
--inputHITSFile ${InputHitsFile} \
--conditionsTag default:OFLCOND-MC16-SDR-25-02 \
--digiSeedOffset1 170 --digiSeedOffset2 170 \
--geometryVersion default:ATLAS-R2-2016-01-00-01 \
--inputHighPtMinbiasHitsFile ${HighPtMinbiasHitsFiles} \
--inputLowPtMinbiasHitsFile ${LowPtMinbiasHitsFiles} \
--jobNumber 1 \
--maxEvents 12 \
--numberOfCavernBkg 0 \
--numberOfHighPtMinBias 0.2595392 \
--numberOfLowPtMinBias 99.2404608 \
--outputRDOFile ${DigiOutFileName} \
--digiSteeringConf 'StandardSignalOnlyTruth' \
--postExec 'all:ServiceMgr.VTuneProfilerService.ResumeEvent=2;' \
--postInclude 'default:PyJobTransforms/UseFrontier.py' 'all:PyJobTransforms/HepMcParticleLinkVerbosity.py' \
--pileupFinalBunch 6 \
--preExec 'all:from AthenaCommon.BeamFlags import jobproperties;jobproperties.Beam.numberOfCollisions.set_Value_and_Lock(20.0);' \
'all:userRunLumiOverride={\"run\":310000, \"startmu\":50.0, \"endmu\":60.0, \"stepmu\":1.0, \"startlb\":1, \"timestamp\":1550000000};' \
--preInclude 'all:PerfMonVTune/VTuneProfileEventLoop_preInclude.py,LArConfiguration/LArConfigRun2Old.py' 'HITtoRDO:Digitization/ForceUseOfPileUpTools.py,SimulationJobOptions/preInlcude.PileUpBunchTrainsMC16c_2017_Config1.py,RunDependentSimData/configEvtNbr_sequential.py,RunDependentSimData/configLumi_muRange.py' \
--skipEvents 0

exit $?
