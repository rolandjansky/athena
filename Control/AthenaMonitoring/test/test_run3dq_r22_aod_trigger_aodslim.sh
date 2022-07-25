#!/bin/bash
# art-description: AOD->HIST, R22 MC, Trigger Only, AODSLIM content (default)
# art-type: grid
# art-memory: 4096
# art-include: master/Athena
# art-include: 22.0/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*
# art-athena-mt: 2

ATHENA_CORE_NUMBER=1 Reco_tf.py --AMI=q445 --preExec 'from AthenaConfiguration.AllConfigFlags import ConfigFlags; ConfigFlags.Trigger.AODEDMSet="AODSLIM"' --outputAODFile=myAOD.pool.root --imf False
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
