#!/bin/bash
# art-description: AOD->HIST, R21 data AOD
# art-type: grid
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root
# art-output: log*

Run3DQTestingDriver.py 'Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/AthenaMonitoring/data18_13TeV.00354309.physics_Main.recon.AOD.f946._lb0130._0001.1"]' DQ.Steering.doHLTMon=False DQ.Environment=AOD  > log.HIST_Creation 2>&1

echo "art-result: $? HIST_Creation"

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
REFFILE=(./ref-*/ExampleMonitorOutput.root)
hist_diff.sh ExampleMonitorOutput.root $REFFILE -x TIME_execute -i > log.HIST_Diff 2>&1
echo "art-result: $? HIST_Diff"
