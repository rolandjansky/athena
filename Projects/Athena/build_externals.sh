#!/bin/bash
#
# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Script building all the externals necessary for Athena.
#

# Set up the variables necessary for the script doing the heavy lifting.
ATLAS_PROJECT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
ATLAS_EXT_PROJECT_NAME="AthenaExternals"
ATLAS_BUILDTYPE="RelWithDebInfo"
ATLAS_EXTRA_CMAKE_ARGS=(-DLCG_VERSION_NUMBER=101
                        -DLCG_VERSION_POSTFIX="_ATLAS_12"
                        -DATLAS_GAUDI_TAG="v36r4.000"
                        -DATLAS_ACTS_TAG="v15.0.1"
                        -DATLAS_ONNXRUNTIME_USE_CUDA=FALSE
                        -DATLAS_GEOMODEL_TAG="ad59da70"
                        -DATLAS_BUILD_CORAL=TRUE)
ATLAS_EXTRA_MAKE_ARGS=()

# Let "the common script" do all the heavy lifting.
source "${ATLAS_PROJECT_DIR}/../../Build/AtlasBuildScripts/build_project_externals.sh"
