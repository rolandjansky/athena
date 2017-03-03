#!/bin/sh
set -e

Reco_tf.py --AMI=q222 --outputTAGFile=myTAG.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root --outputHISTFile=myHIST.root --inputBSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/data12_8TeV.00209109.physics_JetTauEtmiss.merge.RAW._lb0186._SFO-1._0001.1

