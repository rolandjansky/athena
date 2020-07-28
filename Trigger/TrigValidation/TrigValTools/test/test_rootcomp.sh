#!/usr/bin/env bash
# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
#
# Unit test for rootcomp.py

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

# Create histogram files:
assert_pass root -l -b -n -q $1

# Check default options:
assert_pass rootcomp.py hist1.root hist1.root
assert_pass test -f rootcomp.root
assert_pass test -f rootcomp.ps

# Default options for the following specific tests:
opts="--noSkipList --noPS --noRoot"

# Check regular histogram:
assert_pass rootcomp.py hist1.root hist2.root $opts --select h1

# Check label sorting:
assert_pass rootcomp.py hist1.root hist2.root $opts --skip hrandom --sortLabels

# Check failure for random histogram:
assert_fail rootcomp.py hist1.root hist2.root $opts --sortLabels

# Missing references:
assert_fail rootcomp.py hist2.root hist1.root $opts --skip hrandom --sortLabels
assert_pass rootcomp.py hist2.root hist1.root $opts --skip hrandom --sortLabels --ignoreMissingRef

# If we get here all tests succeeded:
exit 0
