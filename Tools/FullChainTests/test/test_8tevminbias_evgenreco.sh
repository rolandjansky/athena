#!/bin/sh
#
# art-description: evgen
# art-type: grid

export JODIR=/cvmfs/atlas.cern.ch/repo/sw/Generators/MC14JobOptions/latest;export JOBOPTSEARCHPATH=$JODIR/share/tests:$JOBOPTSEARCHPATH;Generate_tf.py --ecmEnergy=8000. --runNumber=108351 --firstEvent=1 --maxEvents=5000 --randomSeed=54298752 --jobConfig=MC14.108351.Pythia8_A2CTEQ6L1_minbias_inelastic.py --outputEVNTFile=MC14.108351.pythia_minbias.8TeV.EVNT.pool.root

#
Sim_tf.py --simulator 'MC12G4' --inputEVNTFile 'MC14.108351.pythia_minbias.8TeV.EVNT.pool.root' --outputHitsFile 'MC14.108351.pythia_minbias.8TeV.HITS.pool.root' --maxEvents 10 --skipEvents 0 --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag 'OFLCOND-RUN12-SDR-21' --DataRunNumber '222222' --physicsList 'FTFP_BERT' --enableLooperKiller 'True' --randomSeed 54298752 --preInclude 'EVNTtoHITS:SimulationJobOptions/preInclude.BeamPipeKill.py' --postInclude 'PyJobTransforms/UseFrontier.py' --ignorePatterns='ToolSvc.ISFG4.+ERROR\s+ISF_to_G4Event.+article.conversion.failed'

#
Digi_tf.py --inputHITSFile 'MC14.108351.pythia_minbias.8TeV.HITS.pool.root' --outputRDOFile 'MC14.108351.pythia_minbias.8TeV.RDO.pool.root' --maxEvents '10' --skipEvents '0' --geometryVersion 'ATLAS-R2-2015-03-01-00' --conditionsTag=OFLCOND-RUN12-SDR-31 --digiSeedOffset1 123456 --digiSeedOffset2 2345678 --postInclude 'PyJobTransforms/UseFrontier.py'

#
Reco_tf.py --inputRDOFile 'MC14.108351.pythia_minbias.8TeV.RDO.pool.root' --outputESDFile 'MC14.108351.pythia_minbias.8TeV.ESD.pool.root' --outputAODFile 'MC14.108351.pythia_minbias.8TeV.AOD.pool.root' --maxEvents 10 --skipEvents 0 --autoConfiguration 'everything' --postInclude 'PyJobTransforms/UseFrontier.py'

