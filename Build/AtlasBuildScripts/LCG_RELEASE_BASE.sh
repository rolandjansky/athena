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

    if [ -d /cvmfs/sft.cern.ch/lcg/releases ]; then
        export LCG_RELEASE_BASE=/cvmfs/sft.cern.ch/lcg/releases
    elif [ -d /afs/cern.ch/sw/lcg/releases ]; then
        export LCG_RELEASE_BASE=/afs/cern.ch/sw/lcg/releases
    elif [ -d /cvmfs/atlas.cern.ch/repo/sw/software/21.0/sw/lcg/releases ]; then
        export LCG_RELEASE_BASE=/cvmfs/atlas.cern.ch/repo/sw/software/21.0/sw/lcg/releases
    fi
    echo "Set LCG_RELEASE_BASE = ${LCG_RELEASE_BASE}"

else
    echo "Leaving LCG_RELEASE_BASE = ${LCG_RELEASE_BASE}"
fi
