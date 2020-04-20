#!/bin/bash
# art-description: AOD->HIST, R21 data AOD
# art-type: grid
# art-include: master/Athena
# art-output: ExampleMonitorOutput.root

Run3DQTestingDriver.py 'Input.Files=["/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/ASG/data16_13TeV.00311321.physics_Main.recon.AOD.r9264/AOD.11038520._000001.pool.root.1"]' DQ.Steering.doHLTMon=False DQ.Environment=AOD

echo "art-result: $? HIST_Creation"

ArtPackage=$1
ArtJobName=$2
art.py download ${ArtPackage} ${ArtJobName}
hist_diff.sh ExampleMonitorOutput.root ./ref-*/ExampleMonitorOutput.root -x TIME_execute -i
echo "art-result: $? HIST_Diff"
