# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script used to set up the build environment for AthAnalysis.
#

# Set up the variables necessary for the script doing the heavy lifting.
ATLAS_PROJECT_NAME="AthAnalysis"
ATLAS_PROJECT_DIR=$(cd $(dirname ${BASH_SOURCE[0]}) && pwd)
ATLAS_EXT_PROJECT_NAME="AthAnalysisExternals"

# Let "the common script" do all the heavy lifting.
source "${ATLAS_PROJECT_DIR}/../../Build/AtlasBuildScripts/setup_project_build_env.sh"
