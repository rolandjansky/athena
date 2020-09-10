################################################################################
# Package: MuonChamberT0s
################################################################################

# Declare the package name:
atlas_subdir( MuonChamberT0s )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonChamberT0s
                   src/*.cxx
                   PUBLIC_HEADERS MuonChamberT0s
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthenaKernel Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonChamberT0sDict
                      MuonChamberT0s/MuonChamberT0sDict.h
                      MuonChamberT0s/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} Identifier MuonChamberT0s )

