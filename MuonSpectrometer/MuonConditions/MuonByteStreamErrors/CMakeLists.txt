################################################################################
# Package: MuonByteStreamErrors
################################################################################

# Declare the package name:
atlas_subdir( MuonByteStreamErrors )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonByteStreamErrors
                   src/*.cxx
                   PUBLIC_HEADERS MuonByteStreamErrors
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES AthContainers Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonByteStreamErrorsDict
                      MuonByteStreamErrors/MuonByteStreamErrorsDict.h
                      MuonByteStreamErrors/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} AthContainers Identifier MuonByteStreamErrors )

