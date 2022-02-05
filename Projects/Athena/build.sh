#!/bin/bash
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script for building the Athena project.
#

# Set up the variables necessary for the script doing the heavy lifting.
ATLAS_PROJECT_NAME="Athena"
ATLAS_PROJECT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
ATLAS_BUILDTYPE="RelWithDebInfo"
ATLAS_EXTRA_CMAKE_ARGS=(-DBoost_NO_WARN_NEW_VERSIONS=TRUE)
ATLAS_EXTRA_MAKE_ARGS=()

# Let "the common script" do all the heavy lifting.
source "${ATLAS_PROJECT_DIR}/../../Build/AtlasBuildScripts/build_project.sh"
