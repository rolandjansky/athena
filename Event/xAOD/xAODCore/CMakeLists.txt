# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name.
atlas_subdir( xAODCore )

# Extra dependencies based on what environment we are in.
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs AthenaKernel )
endif()

# External dependencies.
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
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks CxxUtils RootUtils
   ${extra_libs} )

atlas_add_dictionary( xAODCoreDict
   xAODCore/xAODCoreDict.h
   xAODCore/selection.xml
   LINK_LIBRARIES xAODCore )

atlas_add_dictionary( xAODCoreSTLDict
   xAODCore/xAODCoreSTLDict.h
   xAODCore/selectionSTL.xml )

# Install the CMake code from the package:
atlas_install_generic( cmake/xAODUtilitiesConfig.cmake
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR} )
atlas_install_generic( cmake/xAODUtilities*.xml.in
   DESTINATION ${CMAKE_INSTALL_CMAKEDIR}/modules/skeletons )

# Test(s) in the package.
atlas_add_test( ut_xaodcore_class_def_test
   SOURCES test/ut_class_def.cxx
   LINK_LIBRARIES xAODCore )

atlas_add_test( ut_xaodcore_auxselection_test
   SOURCES test/ut_xaodcore_auxselection_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_clearDecorations_test
   SOURCES test/ut_xaodcore_clearDecorations_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_printhelpers_test
   SOURCES test/ut_xaodcore_printhelpers_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_safedeepcopy_test
   SOURCES test/ut_xaodcore_safedeepcopy_test.cxx
   LINK_LIBRARIES AthContainers xAODCore )

atlas_add_test( ut_xaodcore_auxcontainerbase_test
   SOURCES test/ut_xaodcore_auxcontainerbase_test.cxx
   LINK_LIBRARIES AthContainers xAODCore TestTools )

atlas_add_test( ut_xaodcore_auxinfobase_test
   SOURCES test/ut_xaodcore_auxinfobase_test.cxx
   LINK_LIBRARIES AthContainers xAODCore TestTools )

atlas_add_test( ut_xaodcore_shallowcopy_test
   SOURCES test/ut_xaodcore_shallowcopy.cxx
   LINK_LIBRARIES AthContainers AthLinks xAODCore )

atlas_add_test( auxbase_threading_test
   SOURCES test/auxbase_threading_test.cxx
   LINK_LIBRARIES xAODCore )

# Declare the "include tests".
foreach( header AddDVProxy AuxContainerBase AuxSelection AuxCompression BaseInfo
      ClassID_traits ShallowAuxContainer ShallowAuxInfo ShallowCopy
      tools_AuxPersInfo tools_AuxPersVector tools_IOStats tools_PerfStats
      tools_PrintHelpers tools_ReadStats tools_SafeDeepCopy
      tools_TDVCollectionProxy tools_Utils CLASS_DEF )
   atlas_add_test( inc_${header}
      SOURCES test/inc_${header}.cxx
      LINK_LIBRARIES xAODCore )
endforeach()
