# $Id: CMakeLists.txt 744530 2016-05-03 15:37:50Z krasznaa $
################################################################################
# Package: xAODMuon
################################################################################

# Declare the package name:
atlas_subdir( xAODMuon )

# Extra dependencies, based on the build environment:
if( NOT XAOD_ANALYSIS )
   set( extra_deps Tracking/TrkEvent/TrkSegment )
   set( extra_libs TrkSegment )
endif()

# Declare the package's dependencies:
atlas_depends_on_subdirs(
   PUBLIC
   Control/AthContainers
   Control/AthLinks
   Event/xAOD/xAODBase
   Event/xAOD/xAODCaloEvent
   Event/xAOD/xAODCore
   Event/xAOD/xAODPrimitives
   Event/xAOD/xAODTracking
   MuonSpectrometer/MuonIdHelpers
   ${extra_deps} )

# Component(s) in the package:
atlas_add_library( xAODMuon
   xAODMuon/*.h xAODMuon/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODMuon
   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
   LINK_LIBRARIES AthContainers AthLinks xAODBase xAODCaloEvent xAODCore
   xAODPrimitives xAODTracking MuonIdHelpersLib ${extra_libs} )

atlas_add_dictionary( xAODMuonDict
   xAODMuon/xAODMuonDict.h
   xAODMuon/selection.xml
   LINK_LIBRARIES xAODMuon
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( xAODMuon_Muon_test
   SOURCES test/xAODMuon_Muon_test.cxx
   LINK_LIBRARIES xAODMuon )
