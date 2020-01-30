#!/bin/zsh

echo "Starting the run"
root -b -l <<EOF
  .x LoadLibs.C
  .L RunZDCTreeAnalysis2016FermiExp.C
  RunZDCAnalysis("test_afterMerge0426_6M.root", 6000000, 7, 0, 1, 1, 1, 140)
  .q;
EOF
file=done_zdc.txt
touch $file
echo "DONE RUNNING THE SCRIPT"
