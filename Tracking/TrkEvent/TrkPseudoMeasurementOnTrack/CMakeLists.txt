################################################################################
# Package: TrkPseudoMeasurementOnTrack
################################################################################

# Declare the package name:
atlas_subdir( TrkPseudoMeasurementOnTrack )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          PRIVATE
                          GaudiKernel )

# External dependencies:
find_package( Eigen )
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_library( TrkPseudoMeasurementOnTrack
                   src/*.cxx
                   PUBLIC_HEADERS TrkPseudoMeasurementOnTrack
                   INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                   PRIVATE_INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                   LINK_LIBRARIES ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkSurfaces TrkMeasurementBase
                   PRIVATE_LINK_LIBRARIES ${ROOT_LIBRARIES} GaudiKernel )

atlas_add_dictionary( TrkPseudoMeasurementOnTrackDict
                      TrkPseudoMeasurementOnTrack/TrkPseudoMeasurementOnTrackDict.h
                      TrkPseudoMeasurementOnTrack/selection.xml
                      INCLUDE_DIRS ${ROOT_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                      LINK_LIBRARIES ${ROOT_LIBRARIES} ${EIGEN_LIBRARIES} GeoPrimitives EventPrimitives TrkSurfaces TrkMeasurementBase GaudiKernel TrkPseudoMeasurementOnTrack )

