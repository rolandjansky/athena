#!/bin/sh

# art-description: DxAOD MC16a TOPQ1 : top-xaod validation-cuts.txt
# art-type: grid
# art-output: output.root

pwd
echo $AnalysisTop_DIR
echo $WorkDir_DIR
echo "/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/INPUT.ROOT" > input.txt
cp ${AnalysisTop_DIR}/share/validation-cuts.txt ./
top-xaod validation-cuts.txt input.txt