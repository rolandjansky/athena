#!/usr/bin/bash
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# A script used to copy CI artifacts in a common area
#
# This script works only
# - in ctest-based tests (detected by ATLAS_CTEST_TESTNAME)
# - in the CI (detected by gitlabMergeRequestIid).
# Multiple files can be passed to the command
#   CopyCIArtifact.sh FILE...
# Files are copied to
#   /eos/atlas/atlascerngroupdisk/proj-sit/gitlabci/MR<number>_<hash>/<test_name>
#

# exit on failure
set -e

# check if we are running in the CI
if [[ -z ${gitlabMergeRequestIid+x} ]]; then
  echo "Not running in the CI. No files will be copied"
  exit 0
fi

# check if we are running inside a test
if [[ -z ${ATLAS_CTEST_TESTNAME+x} ]]; then
  echo "Not running as part of a test. No files will be copied"
  exit 0
fi

# check if a file was passed
if [[ -z ${1+x} ]]; then
  echo "Error: No file passed to the script" 1>&2
  echo "Usage: CopyCIArtifact.sh FILE..." 1>&2
  exit 1
fi

# check if we have EOS access
base_folder="/eos/atlas/atlascerngroupdisk/proj-sit/gitlabci"
if [[ ! -d "${base_folder}" ]]; then
  echo "Error: CI artifacts area is not accessible" 1>&2
  exit 2
fi

# generate output folder name
output_folder="${base_folder}/MR${gitlabMergeRequestIid}"
if [[ -n ${gitlabMergeRequestLastCommit+x} ]]; then
  output_folder="${output_folder}_${gitlabMergeRequestLastCommit}"
fi
output_folder="${output_folder}/${ATLAS_CTEST_TESTNAME}"

# make the output folder
mkdir -p "${output_folder}"

# copy the file
cp "${@}" "${output_folder}"

echo "Copied '${*}' to '${output_folder}'"
