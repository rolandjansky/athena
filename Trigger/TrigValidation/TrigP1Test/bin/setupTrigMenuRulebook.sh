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

totalPhysP1json=`ls *_PhysicsP1_pp_run3_v1_*.json | wc -l`

if [ $totalPhysP1json -gt 4 ]; then
    echo "ERROR More JSON files than needed"
    exit 1
elif [ $totalPhysP1json -lt 4 ]; then
    echo "ERROR Less JSON files than needed"
    exit 1
fi

cp HLTMenu_PhysicsP1_pp_run3_v1_*.json ${RB_PATH_SCRIPTS}/HLTMenu.json
cp HLTPrescalesSet_PhysicsP1_pp_run3_v1_*.json ${RB_PATH_SCRIPTS}/HLTPrescale.json
cp L1Menu_PhysicsP1_pp_run3_v1_*.json ${RB_PATH_SCRIPTS}/L1Menu.json
cp L1PrescalesSet_PhysicsP1_pp_run3_v1_*.json ${RB_PATH_SCRIPTS}/L1Prescale.json

exit 0
