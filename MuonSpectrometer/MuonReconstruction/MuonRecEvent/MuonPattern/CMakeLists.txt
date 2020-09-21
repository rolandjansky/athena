################################################################################
# Package: MuonPattern
################################################################################

# Declare the package name:
atlas_subdir( MuonPattern )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( MuonPattern
                   src/*.cxx
                   PUBLIC_HEADERS MuonPattern
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaKernel AthLinks AthContainers GeoPrimitives EventPrimitives MuonPrepRawData TrkParameters TrkTrack TrkTruthData TrkExUtils
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} )

atlas_add_dictionary( MuonPatternDict
                      MuonPattern/MuonPatternDict.h
                      MuonPattern/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} AthLinks AthContainers GeoPrimitives EventPrimitives MuonPrepRawData TrkParameters TrkTrack TrkTruthData TrkExUtils MuonPattern )

