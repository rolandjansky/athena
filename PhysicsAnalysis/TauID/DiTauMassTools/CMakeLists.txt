# $Id: CMakeLists.txt 776850 2016-10-06 13:20:48Z krasznaa $

# Declare a name for the package:
atlas_subdir( DiTauMassTools )

# Extra dependencies based on the environment:
set( extra_dep )
if( XAOD_STANDALONE )
   set( extra_dep Control/xAODRootAccess
      Control/AthContainers
      Event/xAOD/xAODJet )
else()
   set( extra_dep GaudiKernel )
endif()

# Set up the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthToolSupport/AsgTools
   PhysicsAnalysis/AnalysisCommon/PATInterfaces
   Event/xAOD/xAODMissingET
   Event/xAOD/xAODEventInfo
   Event/xAOD/xAODBase
   PRIVATE
   Event/xAOD/xAODTau
   ${extra_dep} )

# Find the external(s):
find_package( ROOT COMPONENTS Core RIO Math Physics )

# Component(s) in the package:
atlas_add_library( DiTauMassToolsLib
   DiTauMassTools/*.h Root/*.cxx
   PUBLIC_HEADERS DiTauMassTools
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AsgTools PATInterfaces xAODMissingET xAODEventInfo xAODBase
   PRIVATE_LINK_LIBRARIES xAODTau )

if( NOT XAOD_STANDALONE )
   atlas_add_component( DiTauMassTools
      src/components/*.cxx
      LINK_LIBRARIES GaudiKernel DiTauMassToolsLib )
endif()

atlas_add_dictionary( DiTauMassToolsDict
   DiTauMassTools/DiTauMassToolsDict.h
   DiTauMassTools/selection.xml
   LINK_LIBRARIES DiTauMassToolsLib )

# Test(s) in the package:
if( XAOD_STANDALONE )
   # FIX ME: temporarily disabled as part of the migration of AnalysisBase
   #         to master

   # atlas_add_test( missingmasstool_test
   #    SOURCES test/ut_ditaumasstools_missingmasstool_test.cxx
   #    INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   #    LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers xAODEventInfo xAODJet
   #    xAODTau xAODMissingET xAODRootAccess DiTauMassToolsLib )

   # atlas_add_test( missingmasstool_test_python
   #    SCRIPT test/ut_ditaumasstools_missingmasstool_test_python.py )
endif()

atlas_add_test( mmcTest
   SOURCES test/ut_mmcTest.cxx
   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES ${ROOT_LIBRARIES} DiTauMassToolsLib )
