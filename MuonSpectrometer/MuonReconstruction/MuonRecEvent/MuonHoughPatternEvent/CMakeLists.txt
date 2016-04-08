################################################################################
# Package: MuonHoughPatternEvent
################################################################################

# Declare the package name:
atlas_subdir( MuonHoughPatternEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          DetectorDescription/GeoPrimitives
                          PRIVATE
                          Control/CxxUtils
                          Tracking/TrkDetDescr/TrkDetElementBase
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkPrepRawData )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Hist Core Tree MathCore RIO pthread )

# tag ROOTBasicLibs was not recognized in automatic conversion in cmt2cmake

# Component(s) in the package:
atlas_add_library( MuonHoughPatternEvent
                   src/*.cxx
                   PUBLIC_HEADERS MuonHoughPatternEvent
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} DataModel GeoPrimitives
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} CxxUtils TrkDetElementBase TrkSurfaces TrkPrepRawData )

