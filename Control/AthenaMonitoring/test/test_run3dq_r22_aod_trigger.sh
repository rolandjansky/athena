#!/bin/bash
# art-description: AOD->HIST, R22 MC, Trigger Only
# art-type: grid
# art-memory: 3072
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*

art.py download TrigAnalysisTest test_trigAna_RDOtoAOD_v1Dev_grid.py
AODFILE=(./ref-*/AOD.pool.root)
Run3DQTestingDriver.py 'Input.Files=["'${AODFILE}'"]' DQ.Steering.doHLTMon=True DQ.Environment=AOD --dqOffByDefault > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"
rm -rf ref-*

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/ExampleMonitorOutput.root)
hist_diff.sh ExampleMonitorOutput.root $REFFILE -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"
