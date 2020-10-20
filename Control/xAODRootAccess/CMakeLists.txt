# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# The name of the package:
atlas_subdir( xAODRootAccess )

# Extra dependencies, based on the build environment:
set( extra_libs )
if( NOT XAOD_STANDALONE )
   set( extra_libs SGTools AthenaKernel )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO Net MathCore )

# Generate a CINT dictionary source file:
atlas_add_root_dictionary( xAODRootAccess xAODRootAccessDictSource
   ROOT_HEADERS xAODRootAccess/tools/TTransTrees.h
   xAODRootAccess/tools/xAODTEventBranch.h xAODRootAccess/tools/xAODTEventTree.h
   xAODRootAccess/tools/xAODTMetaBranch.h xAODRootAccess/tools/xAODTMetaTree.h
   xAODRootAccess/tools/TFileChecker.h xAODRootAccess/tools/TFileMerger.h
   xAODRootAccess/MakeTransientTree.h xAODRootAccess/Init.h Root/LinkDef.h
   EXTERNAL_PACKAGES ROOT )

# Build a library that other components can link against:
atlas_add_library( xAODRootAccess
   xAODRootAccess/*.h xAODRootAccess/*.icc
   xAODRootAccess/tools/*.h xAODRootAccess/tools/*.icc
   Root/*.h Root/*.cxx ${xAODRootAccessDictSource}
   PUBLIC_HEADERS xAODRootAccess
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODCore xAODEventFormat
   xAODRootAccessInterfaces ${extra_libs} )

# Build a (Reflex) dictionary library:
atlas_add_dictionary( xAODRootAccessDict
   xAODRootAccess/xAODRootAccessDict.h
   xAODRootAccess/selection.xml
   LINK_LIBRARIES xAODRootAccess )

# Install the Python modules of the package:
atlas_install_python_modules( python/*.py
                              POST_BUILD_CMD ${ATLAS_FLAKE8} )

# Install the checkxAOD.py script, only for the standalone analysis release.
# Athena based releases have a different implementation available to them.
if( XAOD_STANDALONE )
   atlas_install_scripts( scripts/checkxAOD.py )
endif()

# Build the executables of the package:
foreach( exec xAODChecker xAODMerge xAODFileReadTest )
   atlas_add_executable( ${exec} util/${exec}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess )
endforeach()

# Helper macro setting up the compiled tests of the package:
macro( _add_test name )
   atlas_add_test( ${name}
      SOURCES test/${name}.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils AthContainers xAODCore
      xAODRootAccess
      PROPERTIES TIMEOUT 600
      LOG_IGNORE_PATTERN "Opened file:|find dictionary for type|is already in|[0-9]+ bytes" )
endmacro( _add_test )

# Build/run unit tests for the package:
if( NOT SIMULATIONBASE AND NOT GENERATIONBASE )
   _add_test( ut_xaodrootaccess_metadata_test )
   _add_test( ut_xaodrootaccess_athenaMode_test )
endif()
if( NOT GENERATIONBASE )
   _add_test( ut_xaodrootaccess_remap_test )
endif()
_add_test( ut_xaodrootaccess_slimming_test )
_add_test( ut_xaodrootaccess_stats_test )
_add_test( ut_xaodrootaccess_tauxvector_test )
_add_test( ut_xaodrootaccess_tauxstore_test )
_add_test( ut_xaodrootaccess_tauxstore_insertmove_test )
_add_test( ut_xaodrootaccess_tchain_test )
_add_test( ut_xaodrootaccess_tfileaccesstracer_test )
_add_test( ut_xaodrootaccess_tfilemerger_test )
_add_test( ut_xaodrootaccess_tstore_test )
atlas_add_test( ut_xaodrootaccess_tpystore_test
   SCRIPT python
   ${CMAKE_CURRENT_SOURCE_DIR}/test/ut_xaodrootaccess_tpystore_test.py )

if( XAOD_ANALYSIS )
   _add_test( ut_xaodrootaccess_tevent_test )
   _add_test( ut_xaodrootaccess_tevent_copy_test )
   _add_test( ut_xaodrootaccess_transtree_test )
   atlas_add_test( ut_xaodrootaccess_tpyevent_test
      SCRIPT python
      ${CMAKE_CURRENT_SOURCE_DIR}/test/ut_xaodrootaccess_tpyevent_test.py
      PROPERTIES TIMEOUT 600 )
   atlas_add_test( ut_xaodrootaccess_transtree2_test
      SCRIPT python
      ${CMAKE_CURRENT_SOURCE_DIR}/test/ut_xaodrootaccess_transtree2_test.py
      PROPERTIES TIMEOUT 600 )
endif()

if( NOT XAOD_STANDALONE AND NOT GENERATIONBASE )
   _add_test( ut_xaodrootaccess_proxydict_test )
endif()
