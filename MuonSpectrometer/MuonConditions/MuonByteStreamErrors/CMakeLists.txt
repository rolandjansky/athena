################################################################################
# Package: MuonByteStreamErrors
################################################################################

# Declare the package name:
atlas_subdir( MuonByteStreamErrors )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonByteStreamErrors
                   src/*.cxx
                   PUBLIC_HEADERS MuonByteStreamErrors
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools Identifier
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonByteStreamErrorsDict
                      MuonByteStreamErrors/MuonByteStreamErrorsDict.h
                      MuonByteStreamErrors/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools Identifier MuonByteStreamErrors )

