# Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
#
# This helper script is meant to set up a valid value for the
# LCG_RELEASE_BASE environment variable for the builds.
#
# If the environment variable is already set when the script runs, its
# value is not modified. If it isn't set yet, then the script tries to
# check for the existence of a number of standard directories in which
# LCG releases usually reside.
#

if [ -z "${LCG_RELEASE_BASE}" ]; then
    export LCG_RELEASE_BASE=/cvmfs/sft.cern.ch/lcg/releases
    echo "Set LCG_RELEASE_BASE = ${LCG_RELEASE_BASE}"
else
    echo "Leaving LCG_RELEASE_BASE = ${LCG_RELEASE_BASE}"
fi
