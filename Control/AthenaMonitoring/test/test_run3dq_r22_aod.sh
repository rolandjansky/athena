#!/bin/bash
# art-description: AOD->HIST, R22 data AOD
# art-type: grid
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root

art.py download Tier0ChainTests test_q431.sh
AODFILE=(./ref-*/myAOD.pool.root)
Run3DQTestingDriver.py 'Input.Files=["'${AODFILE}'"]' DQ.Environment=AOD DQ.Steering.doHLTMon=False

echo "art-result: $? HIST_Creation"
rm -rf ref-*

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
hist_diff.sh ExampleMonitorOutput.root ./ref-*/ExampleMonitorOutput.root -x TIME_execute -i
echo "art-result: $? HIST_Diff"
