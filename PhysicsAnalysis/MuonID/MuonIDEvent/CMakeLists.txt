################################################################################
# Package: MuonIDEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonIDEvent )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonIDEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonIDEvent
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES CaloGeoHelpers Navigation ParticleEvent JetEvent muonEvent
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonIDEventDict
                      MuonIDEvent/MuonIDEventDict.h
                      MuonIDEvent/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} CaloGeoHelpers Navigation ParticleEvent JetEvent muonEvent MuonIDEvent )

