#!/bin/bash
# art-description: ESD->HIST, R21 data ESD, MT
# art-type: grid
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root

Run3DQTestingDriver.py 'Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/Tier0ChainTests/q431/21.0/myESD.pool.root"]' DQ.Steering.doHLTMon=False Concurrency.NumThreads=1 Concurrency.NumConcurrentEvents=1 --postExec 'cfg.getEventAlgo("LArCollisionTimeAlg").cutIteration=False'

echo "art-result: $? HIST_Creation"

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
hist_diff.sh ExampleMonitorOutput.root ./ref-*/ExampleMonitorOutput.root -x TIME_execute
echo "art-result: $? HIST_Diff"
rm -rf ref-*

art.py download AthenaMonitoring test_run3eq_r21_esd.sh
hist_diff.sh ExampleMonitorOutput.root ./ref-*/ExampleMonitorOutput.root -x TIME_execute
echo "art-result: $? HIST_Diff_Serial"

