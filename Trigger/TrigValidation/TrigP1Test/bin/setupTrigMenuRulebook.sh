#!/bin/bash

echo "Setting up the TrigMenuRulebook"

export ATLAS_LOCAL_ROOT_BASE="/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase"
source $ATLAS_LOCAL_ROOT_BASE/packageSetups/localSetup.sh git

klist -l | grep 'CERN\.CH' > /dev/null
if [ $? -ne 0 ]; then
  echo "ERROR no Kerberos token found, cannot check out rulebook"
  exit 1
fi

git clone https://:@gitlab.cern.ch:8443/atlas-trigger-menu/TrigMenuRulebook.git -o upstream

if [ $? -ne 0 ]; then
  echo "ERROR git clone failed"
  exit 1
fi

RB_PATH_SCRIPTS=TrigMenuRulebook/scripts
RB_PATH_PYTHON=TrigMenuRulebook/python

totalL1PhysP1json=`ls L1*_PhysicsP1_pp_run3_v1_*.json | wc -l`
totalHLTPhysP1json=`ls HLT*_PhysicsP1_pp_run3_v1_*.json | wc -l`

if [ $totalL1PhysP1json -gt 2 ] || [ $totalHLTPhysP1json -gt 3 ]; then
    echo "ERROR More JSON files than needed"
    exit 1
elif [ $totalL1PhysP1json -lt 2 ] || [ $totalHLTPhysP1json -lt 3 ]; then
    echo "ERROR Less JSON files than needed"
    exit 1
fi

# Move to RB scripts dir to set up links
TEST_DIR=${PWD}
cd ${RB_PATH_SCRIPTS}
# input file path, menu, release
./update_inputs.sh ${TEST_DIR} ${AtlasVersion} PhysicsP1_pp_run3_v1
# Return to test working dir
cd ${TEST_DIR}

sed -i -e 's/allowNoOracleConnection\s*=\s*False/allowNoOracleConnection = True/g' ${RB_PATH_PYTHON}/runOptions.py
sed -i -e 's/retryOracleConnection\s*=\s*True/retryOracleConnection = False/g' ${RB_PATH_PYTHON}/runOptions.py

exit 0
