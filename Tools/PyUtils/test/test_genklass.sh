#!/usr/bin/env bash
# Test gen_klass script by creating a package ready for compilation
#
if [ $# -ne 1 ]; then
    echo "test_genklass.sh SOURCE_DIR"
    echo "  Create a testing package 'MyPkg' in SOURCE_DIR"
    exit 1
fi

srcdir=`readlink -f $1`

mkdir -p ${srcdir}/MyPkg/MyPkg
mkdir -p ${srcdir}/MyPkg/src/components

# Create minimal CMakeLists.txt
if [ ! -f ${srcdir}/MyPkg/CMakeLists.txt ]; then
    cat <<EOF > ${srcdir}/MyPkg/CMakeLists.txt
atlas_subdir( MyPkg )
atlas_add_component( MyPkg
                     src/components/*.cxx src/*.cxx
                     LINK_LIBRARIES AthenaBaseComps
                     )
EOF
fi

# Create component registration
if [ ! -f ${srcdir}/MyPkg/src/components/MyPkg_entries.cxx ]; then
    cat <<EOF > ${srcdir}/MyPkg/src/components/MyPkg_entries.cxx
#include "../MyAlg.h"
#include "../MyReAlg.h"
#include "../MyTool.h"
#include "../MySvc.h"

DECLARE_COMPONENT( MyName::MyAlg )
DECLARE_COMPONENT( MyName::MyReAlg )
DECLARE_COMPONENT( MyName::MyTool )
DECLARE_COMPONENT( MyName::MySvc )
EOF
fi

cd ${srcdir}/MyPkg/MyPkg
acmd.py gen-klass --klass IMySvc --pkg MyPkg --type isvc -o IMySvc
acmd.py gen-klass --klass IMyTool --pkg MyPkg --type itool -o IMyTool
cd ${srcdir}/MyPkg/src
acmd.py gen-klass --klass MyName::MyObj --pkg MyPkg --type object -o MyObj
mv MyObj.h ../MyPkg
acmd.py gen-klass --klass MyName::MyAlg --pkg MyPkg --type alg -o MyAlg
acmd.py gen-klass --klass MyName::MyReAlg --pkg MyPkg --type reentrant_alg -o MyReAlg
acmd.py gen-klass --klass MyName::MySvc --pkg MyPkg --type svc --ipkg MyPkg --iklass IMySvc -o MySvc
acmd.py gen-klass --klass MyName::MyTool --pkg MyPkg --type tool --ipkg MyPkg --iklass IMyTool -o MyTool

echo "Testing package created in ${srcdir}/MyPkg"
