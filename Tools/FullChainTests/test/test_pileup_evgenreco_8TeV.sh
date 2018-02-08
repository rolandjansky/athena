#!/bin/sh
#
# art-description: evgen to reco
# art-type: grid

export JODIR=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC14JobOptions/latest;export JOBOPTSEARCHPATH=$JODIR/share/tests:$JOBOPTSEARCHPATH;Generate_tf.py --ecmEnergy=8000. --runNumber=147818 --firstEvent=1 --maxEvents=5000 --randomSeed=54298752 --jobConfig=MC14.147818.Pythia8_AU2CTEQ6L1_Ztautau.py --outputEVNTFile=MC14.147818.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.EVNT.pool.root --postExec='TestHepMC.EffFailThreshold=0.50'

Sim_tf.py --simulator 'MC12G4' --inputEVNTFile 'MC14.147818.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.EVNT.pool.root' --outputHitsFile 'MC14.147818.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.HITS.pool.root' --maxEvents 10 --skipEvents 0 --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-21' --DataRunNumber '222222' --physicsList 'FTFP_BERT' --randomSeed 54298752 --enableLooperKiller 'True' --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' --postInclude 'PyJobTransforms/UseFrontier.py' --ignorePatterns 'ToolSvc.ISFG4.+ERROR\s+ISF_to_G4Event.+article.conversion.failed'

####  To provide LowPtMinbiasHitsFile file to the Digi.
#
export JODIR=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC14JobOptions/latest;export JOBOPTSEARCHPATH=$JODIR/share/tests:$JOBOPTSEARCHPATH;Generate_tf.py --ecmEnergy=8000. --runNumber=108351 --firstEvent=1 --maxEvents=5000 --randomSeed=54298752 --jobConfig=MC14.108351.Pythia8_A2CTEQ6L1_minbias_inelastic.py --outputEVNTFile=MC14.108351.pythia_minbias.8TeV.EVNT.pool.root

#
Sim_tf.py --simulator 'MC12G4' --inputEVNTFile 'MC14.108351.pythia_minbias.8TeV.EVNT.pool.root' --outputHitsFile 'MC14.108351.pythia_minbias.8TeV.HITS.pool.root' --maxEvents 10 --skipEvents 0 --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-21' --DataRunNumber '222222' --physicsList 'FTFP_BERT' --enableLooperKiller 'True' --randomSeed 54298752 --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' --postInclude 'PyJobTransforms/UseFrontier.py' --ignorePatterns='ToolSvc.ISFG4.+ERROR\s+ISF_to_G4Event.+article.conversion.failed'

# To provide cavernHitsFile to the Digi.
AtlasG4_tf.py --inputEVNTFile 'MC14.108351.pythia_minbias.8TeV.EVNT.pool.root' --outputHITSFile 'discard.HITS.pool.root' --outputEVNT_CAVERNTRFile 'MC14.cavbg8TeV.EVNT.pool.root' --maxEvents 5 --skipEvents 0 --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-21' --DataRunNumber='222222' --physicsList 'FTFP_BERT' --randomSeed 5678 --postInclude 'PyJobTransforms/UseFrontier.py'

#
Sim_tf.py --simulator 'MC12G4' --inputEVNT_CAVERNFile 'MC14.cavbg8TeV.EVNT.pool.root' --outputHITSFile 'MC14.cavbg8TeV.HITS.pool.root' --maxEvents -1 --skipEvents 0 --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-21' --DataRunNumber '222222' --physicsList 'FTFP_BERT' --enableLooperKiller 'True' --randomSeed 8765 --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.G4ReadCavern.py' --postInclude 'PyJobTransforms/UseFrontier.py' --ignorePatterns='ToolSvc.ISFG4.+ERROR\s+ISF_to_G4Event.+article.conversion.failed'


##########
Digi_tf.py --inputHITSFile MC14.147818.Pythia8_AU2CTEQ6L1_Ztautau.8TeV.HITS.pool.root --outputRDOFile MC14.Pileup.RDO.pool.root --maxEvents 2 --skipEvents 0 --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-21' --DataRunNumber='222222' --digiSeedOffset1 123456 --digiSeedOffset2 2345678 --LowPtMinbiasHitsFile 'MC14.108351.pythia_minbias.8TeV.HITS.pool.root' --cavernHitsFile 'MC14.cavbg8TeV.HITS.pool.root' --preInclude 'HITtoRDO:SimulationJobOptions/preInclude.Lumi001DigitConfig_450ns.py,Digitization/ForceUseOfPileUpTools.py' --postInclude 'PyJobTransforms/UseFrontier.py'

#
Reco_tf.py --inputRDOFile 'MC14.Pileup.RDO.pool.root' --outputESDFile 'MC14.Pileup.ESD.pool.root' --outputAODFile 'MC14.Pileup.AOD.pool.root' --maxEvents 2 --skipEvents 0 --autoConfiguration 'everything' --preExec 'muonCnvFlags.RpcCablingMode="new"'
