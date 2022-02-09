#!/bin/sh
#
# art-description: Test for bulk processing at Tier0
# art-type: local
# art-include: master/Athena

# There was a database connection problem reported in ATR-24782. Rodney Walker's solution is to use the following export to fix the problem:
export TNS_ADMIN=/cvmfs/atlas.cern.ch/repo/sw/database/DBRelease/current/oracle-admin


Reco_tf.py  \
--AMI f1199  \
--inputBSFile="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/TCT_Run3/data21_comm.00404400.express_express.merge.RAW._lb2497._SFO-ALL._0001.1" \
--maxEvents=100 \
--outputHISTFile="HIST.root" \
--outputAODFile="AOD.root" \
--imf False

rc1=$?
echo "art-result: $rc1 Reco"
