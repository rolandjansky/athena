#!/bin/sh
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# art-description: Run 3 MC reco
# art-output: log.*
# art-athena-mt: 8
# art-type: grid
# art-include: master/Athena

#This is set on grid sites by default, but not on lxplus etc
#Expicitly unsetting it allows this script to have uniform behaviour on both, which
#is useful for deevlopers to rerun the test locally to diagnose problems.
unset ATHENA_CORE_NUMBER

Reco_tf.py --AMI=q445 --athenaopts='--threads=8' --inputHITSFile=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/RecJobTransformTests/mc21_13p6TeV/mc21_13p6TeV.601229.PhPy8EG_A14_ttbar_hdamp258p75_SingleLep.simul.HITS.e8357_e7400_s3775/HITS.27679639._074703.pool.root.1 --outputRDOFile=myRDO.pool.root --outputAODFile=myAOD.pool.root --outputESDFile=myESD.pool.root

RES=$?
echo "art-result: $RES Reco"
