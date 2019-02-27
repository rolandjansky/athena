#!/usr/bin/env bash
# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Configuration testing script for athenaHLT.py
#

test_options=$@

# Cleanup any orphaned processes
trap cleanup INT TERM EXIT
function cleanup {
    sid=`ps -o sess= -p $$`  # our own session id
    pkill -9 --parent 1 --session $sid ipc_server
    pkill -9 --parent 1 --session $sid is_server
}

# We only test the configuration stage, so these dummy values are just fine.
# Specifying run/sor/detmask avoids the COOL lookup.
file="dummy.data"
run="327265"
sortime="1542880952703855872"
detmask="00000000000000000001fffffffffff7"

# Run only config stage (exit immediately via interactive mode) and filter final ptree
# If there was a failure, the exit code will be non-zero
log=test_athenaHLT-${BASHPID}
echo "e" | athenaHLT.py --file ${file} --detector-mask ${detmask} --run-number ${run} --sor-time ${sortime} --interactive ${test_options} &> $log
cat $log | sed -n '/<Configuration>/,/<\/Magnets>/p;/<\/Magnets>/q' | grep '<' | grep -v 'LogRoot'
