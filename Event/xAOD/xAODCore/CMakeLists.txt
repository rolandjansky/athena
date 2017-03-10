# $Id: CMakeLists.txt 793737 2017-01-24 20:11:10Z ssnyder $
################################################################################
# Package: xAODCore
################################################################################

# Declare the package name:
atlas_subdir( xAODCore )

# Extra dependencies based on what environment we are in:
if( XAOD_STANDALONE )
   set( extra_deps PRIVATE Control/AthLinks )
   set( extra_libs )
else()
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
   xAODCore/*.h xAODCore/*.icc xAODCore/tools/*.h Root/*.cxx
   ${xAODCoreDictSource}
   PUBLIC_HEADERS xAODCore
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks CxxUtils
   ${extra_libs} )

atlas_add_dictionary( xAODCoreRflxDict
   xAODCore/xAODCoreRflxDict.h
   xAODCore/selection.xml
   LINK_LIBRARIES xAODCore )

# Test(s) in the package:
atlas_add_test( ut_xaodcore_class_def_test
   SOURCES test/ut_class_def.cxx
   LINK_LIBRARIES xAODCore )

atlas_add_test( ut_xaodcore_auxselection_test
   SOURCES test/ut_xaodcore_auxselection_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_clearDecorations_test
   SOURCES test/ut_xaodcore_clearDecorations_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_floatcompression_test
   SOURCES test/ut_xaodcore_floatcompression_test.cxx
   LINK_LIBRARIES xAODCore )

atlas_add_test( ut_xaodcore_printhelpers_test
   SOURCES test/ut_xaodcore_printhelpers_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_safedeepcopy_test
   SOURCES test/ut_xaodcore_safedeepcopy_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_auxcontainerbase_test
   SOURCES test/ut_xaodcore_auxcontainerbase_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

if( XAOD_STANDALONE )
   atlas_add_test( ut_xaodcore_shallowcopy_test
      SOURCES test/ut_xaodcore_shallowcopy.cxx
      LINK_LIBRARIES AthContainers AthLinks xAODCore )
endif()

# Declare the "include tests":
foreach( header AddDVProxy AuxContainerBase AuxSelection BaseInfo CLASS_DEF
      ClassID_traits ShallowAuxContainer ShallowAuxInfo ShallowCopy
      tools_AuxPersInfo tools_AuxPersVector tools_IOStats tools_PerfStats
      tools_PrintHelpers tools_ReadStats tools_SafeDeepCopy
      tools_TDVCollectionProxy tools_Utils )
   atlas_add_test( inc_${header}
      SOURCES test/inc_${header}.cxx
      LINK_LIBRARIES xAODCore )
endforeach()
