#!/bin/bash
# art-description: AOD->HIST, R22 MC, Trigger Only, AODSLIM content (default)
# art-type: grid
# art-memory: 3072
# art-include: master/Athena
# art-include: 22.0-mc20/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*

Reco_tf.py --AMI=q445 --preExec 'from TriggerJobOpts.TriggerFlags import TriggerFlags;TriggerFlags.AODEDMSet="AODSLIM"' --athenaopts='--threads=1' --outputAODFile=myAOD.pool.root --imf False
echo "art-result: $? AOD_Creation"

Run3DQTestingDriver.py --inputFiles=myAOD.pool.root DQ.Steering.doHLTMon=True DQ.Environment=AOD --dqOffByDefault > log.HIST_Creation 2>&1
echo "art-result: $? HIST_Creation"

rm -rf ref-*

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/ExampleMonitorOutput.root)
hist_diff.sh ExampleMonitorOutput.root $REFFILE -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"
