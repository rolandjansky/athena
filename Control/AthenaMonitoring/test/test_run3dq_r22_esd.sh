#!/bin/bash
# art-description: ESD->HIST, R22 Run 2 data ESD
# art-type: grid
# art-memory: 4096
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*

art.py download Tier0ChainTests test_q431.sh
ESDFILE=(./ref-*/myESD.pool.root)
Run3DQTestingDriver.py --inputFiles=${ESDFILE} DQ.Steering.doHLTMon=False > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"
rm -rf ref-*

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/ExampleMonitorOutput.root)
hist_diff.sh ExampleMonitorOutput.root $REFFILE -x TIME_execute -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"
