#!/usr/bin/bash
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Post-test script for typical reconstruction tests.
#
if [ $# -ne 2 ]; then
    echo "Syntax: `basename $0` digest_reference content_reference"
    exit 1
fi

if [[ ${ATLAS_CTEST_TESTSTATUS} -ne 0 ]]; then
    echo "Test failed. Not running post-processing."
    exit ${ATLAS_CTEST_TESTSTATUS}
fi

input="myAOD.pool.root"
ref_digest=$1
ref_content=$2

# get references:
get_files -data -remove ${ref_digest} ${ref_content}

# xAODDigest:
xAODDigest.py ${input} digest.txt

if ! diff -qs ${ref_digest} digest.txt; then
    echo "TEST WARNING: The xAODDigest diff failed!"
    echo "The output (>) differs from the reference (<)."
    echo "If differences are expected apply the following patch:"
    head -n1 ${ref_digest}
    diff ${ref_digest} digest.txt
    exit 1
fi

# checkFile:
acmd.py chk-file myAOD.pool.root | \
    awk '/---/{flag=1;next}/===/{flag=0}flag' | \
    awk '{print $10}' | sort > content.txt

# TODO: update reference instead of applying trigger exclusions
if ! diff -I '^HLT' -I '^LVL1' -I '^L1' -qs ${ref_content} content.txt; then
  echo "TEST WARNING: The xAOD object content check failed!"
  echo "The output (>) differs from the reference (<)."
  echo "If differences are expected apply the following patch:"
  diff ${ref_content} content.txt
  exit 1
fi

exit 0
