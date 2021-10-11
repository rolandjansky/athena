#!/bin/bash
# art-description: AOD->HIST, R22 MC, Trigger Only
# art-type: grid
# art-memory: 3072
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*

art.py download Tier0ChainTests test_q221_mt1.sh 
AODFILE=(./ref-*/myAOD.pool.root)
Run3DQTestingDriver.py --inputFiles=${AODFILE} DQ.Steering.doHLTMon=True DQ.Environment=AOD --dqOffByDefault > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"
rm -rf ref-*

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/ExampleMonitorOutput.root)
hist_diff.sh ExampleMonitorOutput.root $REFFILE -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"