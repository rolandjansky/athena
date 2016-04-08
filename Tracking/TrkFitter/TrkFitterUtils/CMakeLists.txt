################################################################################
# Package: TrkFitterUtils
################################################################################

# Declare the package name:
atlas_subdir( TrkFitterUtils )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          DetectorDescription/GeoPrimitives
                          DetectorDescription/Identifier
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkExtrapolation/TrkExUtils
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkSegment
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkVertexOnTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# External dependencies:
find_package( CLHEP )
find_package( Eigen )

# Component(s) in the package:
atlas_add_library( TrkFitterUtils
                   src/*.cxx
                   PUBLIC_HEADERS TrkFitterUtils
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS} ${EIGEN_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} ${EIGEN_LIBRARIES} DataModel GeoPrimitives Identifier GaudiKernel TrkSurfaces TrkEventPrimitives TrkEventUtils TrkMeasurementBase TrkParameters TrkExUtils
                   PRIVATE_LINK_LIBRARIES AtlasDetDescr TrkMaterialOnTrack TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkSegment TrkTrack TrkVertexOnTrack TrkExInterfaces TrkToolInterfaces )

