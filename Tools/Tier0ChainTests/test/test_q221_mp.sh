#!/bin/sh
#
# art-description: RecoTrf
# art-type: grid

Reco_tf.py --AMI=q221 --athenaopts='--nprocs=2' --maxEvents=100 --outputRDOFile=myRDO.pool.root --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/mc15_13TeV.410500.PowhegPythia8EvtGen_A14_ttbar_hdamp172p5_nonallhad.simul.HITS.e4797_s2726.50events.pool.root

