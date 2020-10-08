#!/usr/bin/env bash
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Unit test for chainDump.py

if [ -z "$1" ]; then
    echo "Usage: $0 root.C"
    exit 1
fi

# Helpers:
assert_pass() {
    eval $@ || exit 1
}

assert_fail() {
    eval $@ && exit 1
}

# Clean up files
rm -f testChainDump1.root testChainDump2.root TotalEventsProcessed.txt HLTChain.txt HLTDecision.txt chainDump.json

# Create histogram files:
assert_pass root -l -b -n -q $1

# Run chainDump
assert_fail chainDump.py --json --fracTolerance=0.1 --intTolerance=10 \
--totalHists total --countHists chains decisions \
--histDict chains:HLTChain decisions:HLTDecision \
-r testChainDump1.root -f testChainDump2.root

# Print output files
assert_pass cat TotalEventsProcessed.txt
assert_pass cat HLTChain.txt
assert_pass cat HLTDecision.txt
assert_pass cat chainDump.json

# If we get here all tests succeeded:
exit 0
