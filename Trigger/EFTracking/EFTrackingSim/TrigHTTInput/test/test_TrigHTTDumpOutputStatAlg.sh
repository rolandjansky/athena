#!/bin/sh
source `which HTTInputTestSetup.sh`

export HTTInputFile=$HTTLogHitFile
echo "Running on Input file $HTTInputFile"
athena --evtMax=3  TrigHTTInput/HTTDumpOutputStatAlg_jobOptions.py
