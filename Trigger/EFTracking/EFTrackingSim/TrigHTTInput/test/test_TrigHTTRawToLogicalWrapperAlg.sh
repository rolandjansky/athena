#!/bin/sh
source `which HTTInputTestSetup.sh`

export InputHTTRawHitFile=$HTTRawHitFile
export OutputHTTRawHitFile="httsim_loghits_wrap.OUT.root"

athena --evtMax=5  TrigHTTInput/HTTRawToLogicalHitsWrapperAlg_jobOptions.py

echo "Produced file $OutputHTTRawHitFile, now read it back"

export InputHTTLogHitFile=$OutputHTTRawHitFile
athena --evtMax=5 TrigHTTInput/HTTReadLogicalHitsAlg_jobOptions.py




