#!/usr/bin/env bash
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Configuration testing script for athenaHLT.py
#

test_options=$@

# Cleanup any orphaned processes
trap cleanup INT TERM EXIT
function cleanup {
    sid=`ps -o sess= -p $$`  # our own session id
    pkill -9 -P 1 -s $sid ipc_server
    pkill -9 -P 1 -s $sid is_server
}

# Data file not really important, as we only test the configuration stage.
# Specifying run/sor/detmask avoids the COOL lookup.
file="/cvmfs/atlas-nightlies.cern.ch/repo/data/data-art/TrigP1Test/data18_13TeV.00360026.physics_EnhancedBias.merge.RAW._lb0151._SFO-1._0001.1"
run="360026"
sortime="1536143452000000000"
detmask="0000000000000001c10069fffffffff7"

# Run only config stage (exit immediately via interactive mode) and filter final ptree
# If there was a failure, the exit code will be non-zero
log=test_athenaHLT-${BASHPID}
echo "e" | athenaHLT.py --stdcmalloc --file ${file} --detector-mask ${detmask} --run-number ${run} --sor-time ${sortime} --interactive ${test_options} &> $log
cat $log | sed -n '/<Configuration>/,/<\/Magnets>/p;/<\/Magnets>/q' | grep '<' | grep -v 'LogRoot' | sed 's#<\/Configuration>.*#<\/Configuration>#'
