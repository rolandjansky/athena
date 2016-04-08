#!/bin/sh
########################################################
# Set this to the appropriate values on your system
########################################################

TEMPDIR=$PWD

# Set this to the path of your pacman installation
PACMANDIR=/afs/cern.ch/project/gd/apps/atlas/slc3/pacman-3.15

# Set this to the release or patch number you need
RELEASE=12.0.6.5

# Set this to the SITEROOT of your installation
ATLAS_ROOT=/afs/cern.ch/atlas/software/unvalidated/proj

# Set this to something like AtlasProduction for Project Builds
# or leave it blank for Monolithic releases
KVPROJECT=AtlasProduction

# Set this to the base project. Normally the default AtlasOffline is OK
KVBASEPROJECT=AtlasOffline

# Set this to "no" if you don't want to post
# your result to the central KV server
T_POST="yes"

########################################################
#   You shouln't need to touch any of the lines below
########################################################
KVCACHE="http://classis01.roma1.infn.it/pacman/cache"
KVRELEASE="`echo $RELEASE | cut -d "." -f 1-3`"
export T_POST

# Prepare pacman, download KV and the patches
cd $TEMPDIR
cd $PACMANDIR; source setup.sh; cd -
pacman -trust-all-caches \
        -get $KVCACHE:$KVRELEASE/KitValidation
pacman -trust-all-caches \
        -get $KVCACHE:$KVRELEASE/KitValidationPatches
[ $? -eq 0 ] && source setup.sh
if [ "$KVPROJECT" != "" ] ; then
  echo source $ATLAS_ROOT/cmtsite/setup.sh -tag=$KVPROJECT,$KVRELEASE,opt
  source $ATLAS_ROOT/cmtsite/setup.sh -tag=$KVPROJECT,$KVRELEASE,opt
  # Use the specified patch, if available
  if [ "$KVRELEASE" != "$RELEASE" ] ; then
    echo "Using patch release $RELEASE"
    unset CMTPATH
    cd ${SITEROOT}/${KVPROJECT}/${RELEASE}/${KVPROJECT}RunTime/cmt
    source setup.sh
    cd -
  else
    echo "Using plain release $RELEASE"
    # Setup the AtlasProduction patches
    pacman -trust-all-caches \
           -get $KVCACHE:$KVRELEASE/KitValidationPyJTPatches \
           2> /dev/null
    if [ -d $PWD/AtlasProduction ] ; then
      unset CMTPATH
      cd $PWD/AtlasProduction/*/AtlasProductionRunTime/cmt
      source setup.sh 2>/dev/null
      cd -
    fi
  fi
fi

# Run the tests
[ "$KVDISABLE" != "" ] && KVDISABLE_OPTS="--disable $KVDISABLE"
[ "$KVPROJECT" != "" ] && KVPROJECT_OPTS="--project $KVPROJECT"
KVVER="`./KitValidation/*/share/KitValidation -V | grep version | awk '{print $4'} | awk -F. '{printf "%d",$1; for(i=2;i<=NF;i++) printf "%03d", $i;}'`"
if [ $KVVER -ge 1009015 -a "$KVBASEPROJECT" != "" ] ; then
  # This switch is supported only since release 1.9.15 of KV
  KVADDSEARCH=" --search $KVBASEPROJECT"
fi
./KitValidation/*/share/KitValidation -r $RELEASE -p $ATLAS_ROOT -kng  -t $PWD --bw $KVPROJECT_OPTS $KVDISABLE_OPTS $KVADDSEARCH
