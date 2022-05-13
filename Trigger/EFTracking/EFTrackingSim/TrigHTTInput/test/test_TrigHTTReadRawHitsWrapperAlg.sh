#!/bin/sh
source `which HTTInputTestSetup.sh`

export InputHTTRawHitFile=$HTTRawHitFile
export OutputHTTRawHitFile="httsim_rawhits_wrap.OUT.root"
athena --evtMax=5  TrigHTTInput/HTTReadRawHitsWrapperAlg_jobOptions.py

