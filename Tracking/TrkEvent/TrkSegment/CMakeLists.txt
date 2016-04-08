################################################################################
# Package: TrkSegment
################################################################################

# Declare the package name:
atlas_subdir( TrkSegment )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/DataModel
                          Control/SGTools
                          DetectorDescription/Identifier
                          Tracking/TrkEvent/TrkMeasurementBase
                          PRIVATE
                          Control/AthLinks
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkSegment
                   src/*.cxx
                   PUBLIC_HEADERS TrkSegment
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES DataModel SGTools Identifier TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} AthLinks GaudiKernel TrkSurfaces TrkEventPrimitives )

atlas_add_dictionary( TrkSegmentDict
                      TrkSegment/TrkSegmentDict.h
                      TrkSegment/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} DataModel SGTools Identifier TrkMeasurementBase AthLinks GaudiKernel TrkSurfaces TrkEventPrimitives TrkSegment )

