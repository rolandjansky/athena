#!/bin/bash

if [ $# -lt 1 ]; then
	echo "Usage: run_CompileAlgorithmOverview.sh <TestNumber> <AlgorithmName> <OutputFile>"
	echo "All runs to be processed have to have the same test number, e.g. have to be produced by running DQWebdisplay.py <HistFile> TestDisplay <TestNumber>"
else
	echo $2 $3 /afs/cern.ch/user/a/atlasdqm/dqmdisk/han_results/test/$1/*/*/*.root | root -b $TestArea/DataQuality/DataQualityConfigurations/scripts/CompileAlgorithmOverview.C+
fi
