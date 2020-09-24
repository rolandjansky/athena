################################################################################
# Package: MuonDetDescrUtils
################################################################################

# Declare the package name:
atlas_subdir( MuonDetDescrUtils )

# External dependencies:
find_package( ROOT COMPONENTS RIO Tree Core MathCore Hist pthread )

# Component(s) in the package:
atlas_add_library( MuonDetDescrUtils
                   src/*.cxx
                   PUBLIC_HEADERS MuonDetDescrUtils
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES MuonStationIndexLib
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AGDDKernel MuonReadoutGeometry )

atlas_add_executable( MuonSectorMappingTest
                      exe/MuonSectorMappingTest.cxx
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} MuonStationIndexLib MuonDetDescrUtils )

