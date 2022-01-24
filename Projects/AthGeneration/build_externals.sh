#!/bin/bash
#
# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script building all the externals necessary for AthGeneration.
#

# Set up the variables necessary for the script doing the heavy lifting.
ATLAS_PROJECT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
ATLAS_EXT_PROJECT_NAME="AthGenerationExternals"
ATLAS_BUILDTYPE="RelWithDebInfo"
ATLAS_EXTRA_CMAKE_ARGS=(-DLCG_VERSION_NUMBER=101
                        -DLCG_VERSION_POSTFIX="_ATLAS_8"
                        -DATLAS_GAUDI_TAG="v36r4.000"
                        -DATLAS_COOL_TAG="COOL_3_3_9"
                        -DATLAS_GEOMODEL_TAG="ad59da70"
                        -DATLAS_GDB_SOURCE="http://cern.ch/atlas-software-dist-eos/externals/Gdb/gdb-11.1.tar.xz"
                        -DATLAS_GDB_HASH="257cb0f67927f79acf636d8c01e19990")
ATLAS_EXTRA_MAKE_ARGS=()

# Let "the common script" do all the heavy lifting.
source "${ATLAS_PROJECT_DIR}/../../Build/AtlasBuildScripts/build_project_externals.sh"
