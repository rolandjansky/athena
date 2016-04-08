################################################################################
# Package: JetUtils
################################################################################

# Declare the package name:
atlas_subdir( JetUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          AtlasTest/TestTools
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

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread GenVector )

# Component(s) in the package:
atlas_add_library( JetUtils
                   src/*.cxx
                   Root/*.cxx
                   PUBLIC_HEADERS JetUtils
                   INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CaloEvent xAODCaloEvent xAODJet xAODTracking
                   PRIVATE_LINK_LIBRARIES CaloGeoHelpers AthLinks SGTools xAODPFlow TileEvent )

atlas_add_test( JetUtils_CaloQual_test
                SOURCES
                test/JetUtils_CaloQual_test.cxx
                INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                LINK_LIBRARIES ${ROOT_LIBRARIES} TestTools CaloEvent xAODCaloEvent xAODJet xAODTracking CaloGeoHelpers AthLinks SGTools xAODPFlow TileEvent JetUtils
                EXTRA_PATTERNS "no dictionary for class" )

