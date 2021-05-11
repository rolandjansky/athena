# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# Script used to set up the build environment for AnalysisBase.
#

# Set up the variables necessary for the script doing the heavy lifting.
ATLAS_PROJECT_NAME="AnalysisBase"
if [ "x${BASH_SOURCE[0]}" = "x" ]; then
  # Zsh fallback
  ATLAS_PROJECT_DIR=$(cd "$(dirname "$(print -P %x)")" && pwd)
else
  # Bash
  ATLAS_PROJECT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
fi
ATLAS_EXT_PROJECT_NAME="AnalysisBaseExternals"

# Let "the common script" do all the heavy lifting.
source "${ATLAS_PROJECT_DIR}/../../Build/AtlasBuildScripts/setup_project_build_env.sh"
