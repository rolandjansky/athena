#!/bin/sh
#+
#	Script to setup the runtime cache for the current project, release and $CMTCONFIG version.
#
#   The cache is created by running the:
#
#       cmt -requirements setup >& outfile
#
#   command to create a cache requirements file, and "cmt config" is run on this to create equivalent
#   setup.[c]sh files. These files are saved in the:
#
#       <package>/$CMTCONFIG/runtime/cmt
#
#   creating them if necessary, and replacing older versions. 
#
#   On entry to this script, ${PWD} is set to the cmt directory of the relevant XXXRuntime package.
#
#	Author:-
#	David Quarrie
#-

if [ ! -d ../$CMTCONFIG/runtime ]; then
    \mkdir -p ../$CMTCONFIG/runtime
fi
if [ -f ../$CMTCONFIG/runtime/cache.sh ]; then
    \rm -f ../$CMTCONFIG/runtime/cache.sh
fi
if [ -f ../$CMTCONFIG/runtime/cache.csh ]; then
    \rm -f ../$CMTCONFIG/runtime/cache.csh
fi
if [ ! -d ../$CMTCONFIG/runtime/cmt ]; then
    \mkdir -p ../$CMTCONFIG/runtime/cmt
fi
if [ -f ../$CMTCONFIG/runtime/cmt/requirements ]; then
    \rm -f ../$CMTCONFIG/runtime/cmt/requirements
fi
if [ ! -f ../$CMTCONFIG/runtime/cmt/version.cmt ]; then
    echo "v1" > ../$CMTCONFIG/runtime/cmt/version.cmt
fi
# We disable warnings from CMT only because DBRelease is no longer installed with release by default
CMTEXTRATAGS=useDBRelease,${CMTEXTRATAGS-} cmt -disable_warnings -requirements setup -no_cleanup > ../$CMTCONFIG/runtime/cmt/requirements
pushd ../$CMTCONFIG/runtime/cmt >& /dev/null
cmt config >& /dev/null
popd >& /dev/null

# Standalone setup script making use of cached requirements file above
# compiler setup
gcc_path=`which gcc`
if [ -n "$gcc_path" ] && [ "${gcc_path%/AtlasSite/*}" != "$gcc_path" ]; then
gcc_setup=${gcc_path%/AtlasSite/*}/AtlasSite/cmt/setup_site_gcc
[ -r "${gcc_setup}.sh" ] || unset gcc_setup
fi
if [ -n "$gcc_path" ] && [ -z "${gcc_setup+T}" ] && [ -d "${SITEROOT:-}" ]; then
gcc_setup="${SITEROOT:-}"/AtlasSite/cmt/setup_site_gcc
[ -r "${gcc_setup}.sh" ] || unset gcc_setup
fi
if [ -n "$gcc_path" ] && [ -z "${gcc_setup+T}" ]; then
gcc_setup=$(dirname $(dirname ${gcc_path}))/setup
[ -r "${gcc_setup}.sh" ] || unset gcc_setup
fi
# package root
package_root=$(dirname $PWD)
# 
cat >../$CMTCONFIG/runtime/cmt/standalone-setup.sh <<EOF
${gcc_setup+. ${gcc_setup}.sh}
export CMTROOT=${CMTROOT}
export CMTSITE=${CMTSITE%DISTCC}
export CMTCONFIG=${CMTCONFIG}
export CMTPROJECTPATH=${CMTPROJECTPATH}
${CMTUSERCONTEXT+export CMTUSERCONTEXT=${CMTUSERCONTEXT}}
${CMTHOME+export CMTHOME=${CMTHOME}}
${CMTSTRUCTURINGSTYLE+export CMTSTRUCTURINGSTYLE=${CMTSTRUCTURINGSTYLE}}
export CMTEXTRATAGS=useDBRelease,${CMTEXTRATAGS-}
export CMTHEADVERSION="${CMTHEADVERSION}"
${SITEROOT+export SITEROOT=${SITEROOT}}

export GAUDI_NO_STRIP_PATH=1
export ATLAS_NO_SETUP_MANPATH=1
export ATLAS_NO_SETUP_RUNTIME_HELPER=1
. ${package_root}/${CMTCONFIG}/runtime/cmt/setup.sh
EOF

cat >../$CMTCONFIG/runtime/cmt/standalone-init.sh <<EOF
${gcc_setup+. ${gcc_setup}.sh}
export CMTROOT=${CMTROOT}
export CMTSITE=${CMTSITE%DISTCC}
export CMTCONFIG=${CMTCONFIG}
export CMTPROJECTPATH=${CMTPROJECTPATH}
${CMTUSERCONTEXT+export CMTUSERCONTEXT=${CMTUSERCONTEXT}}
${CMTHOME+export CMTHOME=${CMTHOME}}
${CMTSTRUCTURINGSTYLE+export CMTSTRUCTURINGSTYLE=${CMTSTRUCTURINGSTYLE}}
export CMTEXTRATAGS=useDBRelease,${CMTEXTRATAGS-}
export CMTHEADVERSION="${CMTHEADVERSION}"
${SITEROOT+export SITEROOT=${SITEROOT}}

export GAUDI_NO_STRIP_PATH=1
export ATLAS_NO_SETUP_MANPATH=1
export ATLAS_NO_SETUP_RUNTIME_HELPER=1
. \${CMTROOT}/mgr/setup.sh
EOF

cat >../$CMTCONFIG/runtime/cmt/standalone-setup.csh <<EOF
${gcc_setup+source ${gcc_setup}.csh}
setenv CMTROOT ${CMTROOT}
setenv CMTSITE ${CMTSITE%DISTCC}
setenv CMTCONFIG ${CMTCONFIG}
setenv CMTPROJECTPATH ${CMTPROJECTPATH}
${CMTUSERCONTEXT+setenv CMTUSERCONTEXT ${CMTUSERCONTEXT}}
${CMTHOME+setenv CMTHOME ${CMTHOME}}
${CMTSTRUCTURINGSTYLE+setenv CMTSTRUCTURINGSTYLE ${CMTSTRUCTURINGSTYLE}}
setenv CMTEXTRATAGS useDBRelease,${CMTEXTRATAGS-}
setenv CMTHEADVERSION "${CMTHEADVERSION}"
${SITEROOT+setenv SITEROOT ${SITEROOT}}

setenv GAUDI_NO_STRIP_PATH 1
setenv ATLAS_NO_SETUP_MANPATH 1
setenv ATLAS_NO_SETUP_RUNTIME_HELPER 1
source ${package_root}/${CMTCONFIG}/runtime/cmt/setup.csh
EOF

cat >../$CMTCONFIG/runtime/cmt/standalone-init.csh <<EOF
${gcc_setup+source ${gcc_setup}.csh}
setenv CMTROOT ${CMTROOT}
setenv CMTSITE ${CMTSITE%DISTCC}
setenv CMTCONFIG ${CMTCONFIG}
setenv CMTPROJECTPATH ${CMTPROJECTPATH}
${CMTUSERCONTEXT+setenv CMTUSERCONTEXT ${CMTUSERCONTEXT}}
${CMTHOME+setenv CMTHOME ${CMTHOME}}
${CMTSTRUCTURINGSTYLE+setenv CMTSTRUCTURINGSTYLE ${CMTSTRUCTURINGSTYLE}}
setenv CMTEXTRATAGS useDBRelease,${CMTEXTRATAGS-}
setenv CMTHEADVERSION "${CMTHEADVERSION}"
${SITEROOT+setenv SITEROOT ${SITEROOT}}

setenv GAUDI_NO_STRIP_PATH 1
setenv ATLAS_NO_SETUP_MANPATH 1
setenv ATLAS_NO_SETUP_RUNTIME_HELPER 1
source \${CMTROOT}/mgr/setup.csh
EOF
# end of Standalone setup making use of cached requirements file above

# Setup AtlasSetup fast runtime setup cache
package=`cmt show macro_value package`
export AtlasArea=`cmt show macro_value ${package}_cmtpath`
export AtlasVersion=`basename $AtlasArea`
area=`dirname $AtlasArea`
export AtlasBaseDir=`dirname $area`
cmtsite=$CMTSITE
if [ "$CMTSITE" = "CERNDISTCC" ]; then
    export CMTSITE="CERN"
fi

asetup="/afs/cern.ch/atlas/software/dist/AtlasSetup"
if [ -n "${AtlasSetup:+x}" ]; then
    asetup=${AtlasSetup}
fi
echo "Setup runtime cache - AtlasSetup:${AtlasSetup}, using:${asetup}"

source ${asetup}/scripts/asetup.sh --environment --notest --brief --simulate --asfastlevel 1 --cache --shell=../$CMTCONFIG/runtime/cache.sh
echo "asetup status:$?"
source ${asetup}/scripts/asetup.sh --environment --notest --brief --simulate --asfastlevel 1 --cache --shell=../$CMTCONFIG/runtime/cache.csh
echo "asetup status:$?"

# Note that the following overrides for --usedcache and --usedpm aren't correct yet since the environment isn't correctly setup.
# I need to think a bit more about how to do this properly.
extras=$CMTEXTRATAGS
export CMTEXTRATAGS="${extras},useDCACHE"
source ${asetup}/scripts/asetup.sh --environment --notest --brief --simulate --asfastlevel 1 --cache --shell=../$CMTCONFIG/runtime/cache.nocastor.usedcache.sh
echo "asetup status:$?"
source ${asetup}/scripts/asetup.sh --environment --notest --brief --simulate --asfastlevel 1 --cache --shell=../$CMTCONFIG/runtime/cache.nocastor.usedcache.csh
echo "asetup status:$?"
export CMTEXTRATAGS="${extras},useDPM"
source ${asetup}/scripts/asetup.sh --environment --notest --brief --simulate --asfastlevel 1 --cache --shell=../$CMTCONFIG/runtime/cache.usedpm.sh
echo "asetup status:$?"
source ${asetup}/scripts/asetup.sh --environment --notest --brief --simulate --asfastlevel 1 --cache --shell=../$CMTCONFIG/runtime/cache.usedpm.csh
echo "asetup status:$?"
date
if [ "$cmtsite" = "CERNDISTCC" ]; then
    export CMTSITE=$cmtsite
fi
export CMTEXTRATAGS=$extras

