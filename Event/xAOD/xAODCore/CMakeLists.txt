# $Id: CMakeLists.txt 767089 2016-08-09 14:55:53Z wlampl $
################################################################################
# Package: xAODCore
################################################################################

# Declare the package name:
atlas_subdir( xAODCore )

# Extra dependencies based on what environment we are in:
if( NOT XAOD_STANDALONE )
   set( extra_deps Control/SGTools )
   set( extra_libs SGTools )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthContainersInterfaces
   Control/AthLinks
   Control/CxxUtils
   ${extra_deps} )

# External dependencies:
find_package( ROOT COMPONENTS Core Hist Tree RIO )

# Component(s) in the package:
atlas_add_root_dictionary( xAODCore
   xAODCoreDictSource
   ROOT_HEADERS xAODCore/tools/ReadStats.h xAODCore/tools/PerfStats.h
   Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

atlas_add_library( xAODCore
   xAODCore/*.h xAODCore/*.h xAODCore/tools/*.h Root/*.cxx
   ${xAODCoreDictSource}
   PUBLIC_HEADERS xAODCore
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks ${extra_libs} )

atlas_add_dictionary( xAODCoreRflxDict
   xAODCore/xAODCoreRflxDict.h
   xAODCore/selection.xml
   LINK_LIBRARIES xAODCore )

# Test(s) in the package:
atlas_add_test( ut_xaodcore_auxselection_test
   SOURCES test/ut_xaodcore_auxselection_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_printhelpers_test
   SOURCES test/ut_xaodcore_printhelpers_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )
