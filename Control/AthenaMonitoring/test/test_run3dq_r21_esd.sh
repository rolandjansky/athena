#!/bin/bash
# art-description: ESD->HIST, R21 data ESD, MT
# art-type: grid
# art-memory: 4096
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*

Run3DQTestingDriver.py --inputFiles=/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/AthenaMonitoring/q431/21.0/f946/myESD.pool.root DQ.Steering.doHLTMon=False IOVDb.GlobalTag=\"CONDBR2-BLKPA-RUN2-06\" > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/ExampleMonitorOutput.root)
hist_diff.sh ExampleMonitorOutput.root $REFFILE -x TIME_execute -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"
rm -rf ref-*

art.py download AthenaMonitoring test_run3dq_r21_esd.sh
hist_diff.sh ExampleMonitorOutput.root $REFFILE -x TIME_execute -i > log.HIST_Diff_Serial 2>&1
echo "art-result: $? HIST_Diff_Serial"

