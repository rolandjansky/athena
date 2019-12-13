# Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
#
# Add the setupATLAS command, as a BASH function, to the environment.
#

setupATLAS() {
    export ATLAS_LOCAL_ROOT_BASE=/cvmfs/atlas.cern.ch/repo/ATLASLocalRootBase
    source $ATLAS_LOCAL_ROOT_BASE/user/atlasLocalSetup.sh
}
