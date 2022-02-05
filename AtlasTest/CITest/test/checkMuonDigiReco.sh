#!/usr/bin/env bash
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration

if [[ ${ATLAS_CTEST_TESTSTATUS} -ne 0 ]]; then
    echo "Test failed. Not running post-processing."
    exit ${ATLAS_CTEST_TESTSTATUS}
fi

# ensure that the output ESD has all types of muon technologies present
echo "Checking reconstruction output..."
checkxAOD.py OUT_ESD.root | grep Container > content.txt
for value in RPC_Measurements TGC_Measurements STGC_Measurements MM_Measurements CSC_Measurements MDT_DriftCircles
do
  if grep -q ${value} "content.txt"; then
    echo "${value} present"
  else
    echo "${value} not found, exit"
    exit 1
  fi
done

exit 0
