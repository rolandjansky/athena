# $Id: CMakeLists.txt 790886 2016-12-22 22:03:35Z ssnyder $
################################################################################
# Package: xAODMuon
################################################################################

# Declare the package name:
atlas_subdir( xAODMuon )

# Extra dependencies, based on the build environment:
if( NOT XAOD_ANALYSIS AND NOT GENERATIONBASE )
   set( extra_deps Tracking/TrkEvent/TrkSegment )
   set( extra_libs TrkSegment )
endif()

if (BUILDVP1LIGHT)
    if( BUILDVP1LIGHT_DIST STREQUAL "ubuntu")
        set( extra_libs ${extra_libs} GenVector )
    endif()
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
   MuonSpectrometer/MuonStationIndex
   ${extra_deps} )

find_package( ROOT COMPONENTS Core GenVector )

# Component(s) in the package:
atlas_add_library( xAODMuon
   xAODMuon/*.h xAODMuon/versions/*.h Root/*.cxx
   PUBLIC_HEADERS xAODMuon
   LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers AthLinks xAODBase xAODCaloEvent xAODCore
   xAODPrimitives xAODTracking MuonStationIndexLib ${extra_libs} )

atlas_add_dictionary( xAODMuonDict
   xAODMuon/xAODMuonDict.h
   xAODMuon/selection.xml
   LINK_LIBRARIES xAODMuon
   EXTRA_FILES Root/dict/*.cxx )

atlas_add_test( xAODMuon_Muon_test
   SOURCES test/xAODMuon_Muon_test.cxx
   LINK_LIBRARIES xAODMuon )
