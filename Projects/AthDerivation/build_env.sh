# Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
#
# Script used to set up the build environment for AthDerivation.
#

# Set up the variables necessary for the script doing the heavy lifting.
ATLAS_PROJECT_NAME="AthDerivation"
if [ "x${BASH_SOURCE[0]}" = "x" ]; then
  # Zsh fallback
  ATLAS_PROJECT_DIR=$(cd "$(dirname "$(print -P %x)")" && pwd)
else
  # Bash
  ATLAS_PROJECT_DIR=$(cd "$(dirname "${BASH_SOURCE[0]}")" && pwd)
fi
ATLAS_EXT_PROJECT_NAME="AthDerivationExternals"

# Let "the common script" do all the heavy lifting.
source "${ATLAS_PROJECT_DIR}/../../Build/AtlasBuildScripts/setup_project_build_env.sh"
