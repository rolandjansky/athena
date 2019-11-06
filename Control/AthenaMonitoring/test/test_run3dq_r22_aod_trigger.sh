#!/bin/bash
# art-description: AOD->HIST, R22 MC, Trigger Only
# art-type: grid
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root

art.py download TrigAnalysisTest test_trigAna_q221_RDOtoAOD_mt1_grid.sh
AODFILE=(./ref-*/AOD.pool.root)
Run3DQTestingDriver.py 'Input.Files=["'${AODFILE}'"]' DQ.Steering.doHLTMon=True --dqOffByDefault

echo "art-result: $? HIST_Creation"
rm -rf ref-*

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
hist_diff.sh ExampleMonitorOutput.root ./ref-*/ExampleMonitorOutput.root
echo "art-result: $? HIST_Diff"
