################################################################################
# Package: TrkTruthData
################################################################################

# Declare the package name:
atlas_subdir( TrkTruthData )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          Generators/GeneratorObjects
                          Tracking/TrkEvent/TrkTrack )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkTruthData
                   PUBLIC_HEADERS TrkTruthData
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} DataModel SGTools GeoPrimitives Identifier GeneratorObjects TrkTrack )

atlas_add_dictionary( TrkTruthDataDict
                      src/TrkTruthDataDict.h
                      TrkTruthData/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} TrkTruthData )

