# $Id: CMakeLists.txt 794799 2017-01-31 17:07:29Z ssnyder $
################################################################################
# Package: TrigConfxAOD
################################################################################

# Declare the package name:
atlas_subdir( TrigConfxAOD )

# Extra dependencies, based on the environment:
set( extra_deps )
if( XAOD_STANDALONE )
   set( extra_deps Control/xAODRootAccess )
else()
   set( extra_deps Control/AthenaBaseComps Control/AthenaKernel
      Control/StoreGate GaudiKernel )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   Event/xAOD/xAODTrigger
   Trigger/TrigConfiguration/TrigConfHLTData
   Trigger/TrigConfiguration/TrigConfInterfaces
   Trigger/TrigConfiguration/TrigConfL1Data
   PRIVATE
   ${extra_deps} )

# External(s):
find_package( ROOT COMPONENTS Core RIO )

# Libraries in the package:
atlas_add_library( TrigConfxAODLib
   TrigConfxAOD/*.h Root/*.cxx
   PUBLIC_HEADERS TrigConfxAOD
   LINK_LIBRARIES AsgTools xAODTrigger TrigConfL1Data TrigConfHLTData
   TrigConfInterfaces )

if( NOT XAOD_STANDALONE )
   atlas_add_component( TrigConfxAOD
      src/*.h src/*.cxx src/components/*.cxx
      LINK_LIBRARIES AthenaBaseComps AthenaKernel StoreGateLib GaudiKernel
      TrigConfxAODLib )
endif()

atlas_add_dictionary( TrigConfxAODDict
   TrigConfxAOD/TrigConfxAODDict.h
   TrigConfxAOD/selection.xml
   LINK_LIBRARIES TrigConfxAODLib )

# Executable(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_executable( xAODTrigConfReadTest
      util/xAODTrigConfReadTest.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess AsgTools TrigConfxAODLib )
endif()

# Test(s) in the package:
if( XAOD_STANDALONE )
   atlas_add_test( xaodconfigtool_test
      SCRIPT test/ut_trigconfxaod_xaodconfigtool_test.py )
endif()

# Install files from the package:
atlas_install_joboptions( share/*.py )
atlas_install_scripts( scripts/*.py )
