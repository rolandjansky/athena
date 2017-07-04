# $Id: CMakeLists.txt 801794 2017-03-29 15:34:20Z elmsheus $
################################################################################
# Package: JetUtils
################################################################################

# Declare the package name:
atlas_subdir( JetUtils )

# Declare the package's dependencies:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_depends_on_subdirs(
      PUBLIC
      Event/xAOD/xAODCaloEvent
      Event/xAOD/xAODJet
      Event/xAOD/xAODTracking
      PRIVATE
      Calorimeter/CaloGeoHelpers
      Event/xAOD/xAODPFlow
      Control/AthLinks )
else()
   atlas_depends_on_subdirs(
      PUBLIC
      Calorimeter/CaloEvent
      Event/xAOD/xAODCaloEvent
      Event/xAOD/xAODJet
      Event/xAOD/xAODTracking
      PRIVATE
      Calorimeter/CaloGeoHelpers
      Control/AthLinks
      Control/SGTools
      Event/xAOD/xAODPFlow
      TileCalorimeter/TileEvent )
endif()

# External dependencies:
find_package( ROOT COMPONENTS Core Tree RIO )

# Component(s) in the package:
if( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_library( JetUtils
      JetUtils/*.h JetUtils/*.icc Root/*.cxx
      PUBLIC_HEADERS JetUtils
      LINK_LIBRARIES xAODCaloEvent xAODJet xAODTracking
      PRIVATE_LINK_LIBRARIES CaloGeoHelpers xAODPFlow )
else()
   atlas_add_library( JetUtils
      JetUtils/*.h JetUtils/*.icc Root/*.cxx src/*.cxx
      PUBLIC_HEADERS JetUtils
      LINK_LIBRARIES CaloEvent xAODCaloEvent xAODJet xAODTracking
      PRIVATE_LINK_LIBRARIES CaloGeoHelpers xAODPFlow TileEvent )
endif()

# Test(s) in the package:
if( NOT XAOD_STANDALONE )
   atlas_add_test( JetUtils_CaloQual_test
      SOURCES test/JetUtils_CaloQual_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODCaloEvent JetUtils AthLinks SGTools
      EXTRA_PATTERNS "no dictionary for class" )
elseif( XAOD_STANDALONE OR XAOD_ANALYSIS )
   atlas_add_test( JetUtils_CaloQual_test
      SOURCES test/JetUtils_CaloQual_test.cxx
      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
      LINK_LIBRARIES ${ROOT_LIBRARIES} xAODRootAccess xAODCaloEvent JetUtils AthLinks 
      EXTRA_PATTERNS "no dictionary for class" )
endif()

