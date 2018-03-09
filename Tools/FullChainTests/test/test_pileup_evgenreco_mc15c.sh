#!/bin/sh
#
# art-description: evgen to reco
# art-type: grid

function findinput(){
  if [ ! -f $1 ]; then
    echo $2
  else
    echo $1
  fi
}

# INPUTS and backups
MY_EVGEN_zhad='MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.EVNT.pool.root'
BK_EVGEN_zhad='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/FullChainTests/MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.EVNT.pool.root'

MY_EVGEN_minbias='MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.13TeV.EVNT.pool.root'
BK_EVGEN_minbias='/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.13TeV.EVNT.pool.root'


export JODIR=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest;export JOBOPTSEARCHPATH=$JODIR/share/DSID304xxx:$JOBOPTSEARCHPATH; Generate_tf.py --ecmEnergy 13000. --runNumber 304628 --firstEvent 1 --maxEvents 10 --randomSeed 54298752 --jobConfig MC15JobOptions/MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.py --outputEVNTFile ${MY_EVGEN_zhad} --postExec 'TestHepMC.EffFailThreshold=0.50'

# check EVGEN output OK
MY_EVGEN_zhad=$(findinput MY_EVGEN_zhad BK_EVGEN_zhad)

#
Sim_tf.py --conditionsTag "default:OFLCOND-RUN12-SDR-19" --DataRunNumber 222525 --DBRelease "current" --geometryVersion "default:ATLAS-R2-2015-03-01-00_VALIDATION" --physicsList FTFP_BERT --postInclude "default:PyJobTransforms/UseFrontier.py" --preInclude "EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py" --runNumber=304628 --simulator MC12G4 --truthStrategy MC15aPlus --inputEVNTFile ${MY_EVGEN_zhad} --outputHITSFile MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.HITS.pool.root --maxEvents 10

#### To provide input for the LowPtMinbiasHitsFile in Reco
#
export JODIR=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC15JobOptions/latest;export JOBOPTSEARCHPATH=$JODIR/share/DSID361xxx:$JOBOPTSEARCHPATH; Generate_tf.py --ecmEnergy=13000. --runNumber=361033 --firstEvent=1 --maxEvents=5000 --randomSeed=8917284 --jobConfig=MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.py --outputEVNTFile=${MY_EVGEN_minbias}

# check EVGEN output OK
MY_EVGEN_minbias=$(findinput MY_EVGEN_minbias BK_EVGEN_minbias)

#
Sim_tf.py --conditionsTag "default:OFLCOND-RUN12-SDR-19" --DataRunNumber 222525 --DBRelease "current" --geometryVersion "default:ATLAS-R2-2015-03-01-00_VALIDATION" --physicsList FTFP_BERT --postInclude "default:PyJobTransforms/UseFrontier.py" --preInclude "EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.FrozenShowersFCalOnly.py" --runNumber=304628 --simulator MC12G4 --truthStrategy MC15aPlus --inputEVNTFile ${MY_EVGEN_minbias} --outputHITSFile MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.13TeV.HITS.pool.root --maxEvents=10

##### To provide input for the cavernHitsFile
AtlasG4_tf.py --inputEVNTFile ${MY_EVGEN_minbias} --outputHITSFile 'discard.HITS.pool.root' --outputEVNT_CAVERNTRFile 'MC15.cavbg13TeV.EVNT.pool.root' --maxEvents 10 --skipEvents 0 --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'default:OFLCOND-RUN12-SDR-19' --DataRunNumber='222525' --physicsList 'FTFP_BERT' --randomSeed 5678 --preInclude 'SimulationJobOptions/preInclude.G4WriteCavern.py' --postInclude 'PyJobTransforms/UseFrontier.py'

#
Sim_tf.py --simulator 'MC12G4' --inputEVNT_CAVERNFile 'MC15.cavbg13TeV.EVNT.pool.root' --outputHITSFile 'MC15.cavbg13TeV.HITS.pool.root' --maxEvents -1 --skipEvents 0 --geometryVersion 'default:ATLAS-R2-2015-03-01-00_VALIDATION' --conditionsTag 'default:OFLCOND-RUN12-SDR-19' --DataRunNumber '222525' --physicsList 'FTFP_BERT' --enableLooperKiller 'True' --randomSeed 8765 --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py,SimulationJobOptions/preInclude.G4ReadCavern.py' --postInclude 'PyJobTransforms/UseFrontier.py' --ignorePatterns='ToolSvc.ISFG4.+ERROR\s+ISF_to_G4Event.+article.conversion.failed'


#### Reco
Reco_tf.py --inputHITSFile MC15.304628.Pythia8EvtGen_A14NNPDF23LO_ZHad_280_500.13TeV.HITS.pool.root --outputRDOFile MC15.Pileup.RDO.pool.root --outputESDFile 'MC15.Pileup.ESD.pool.root' --outputAODFile 'MC15.Pileup.AOD.pool.root' --maxEvents 2 --jobNumber 1 --skipEvents 0 --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-21' --DataRunNumber='222525' --digiSeedOffset1 123456 --digiSeedOffset2 2345678 --LowPtMinbiasHitsFile 'MC15.361033.Pythia8EvtGen_A2MSTW2008LO_minbias_inelastic.13TeV.HITS.pool.root' --cavernHitsFile 'MC15.cavbg13TeV.HITS.pool.root' --preInclude 'HITtoRDO:SimulationJobOptions/preInclude.PileUpBunchTrainsMC15_2015_25ns_Config1.py,Digitization/ForceUseOfPileUpTools.py,RunDependentSimData/configLumi_run284500_v2.py' --postInclude 'PyJobTransforms/UseFrontier.py'

