################################################################################
# Package: InDetTrackSplitterTool
################################################################################

# Declare the package name:
atlas_subdir( InDetTrackSplitterTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkTrack
                          PRIVATE
                          DetectorDescription/GeoPrimitives
                          Event/EventPrimitives
                          InnerDetector/InDetDetDescr/InDetIdentifier
                          InnerDetector/InDetRecEvent/InDetRIO_OnTrack
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkFitter/TrkFitterInterfaces )

# External dependencies:
find_package( Eigen )

# Component(s) in the package:
atlas_add_component( InDetTrackSplitterTool
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${EIGEN_INCLUDE_DIRS}
                     LINK_LIBRARIES ${EIGEN_LIBRARIES} AthenaBaseComps GaudiKernel TrkTrack GeoPrimitives EventPrimitives InDetIdentifier InDetRIO_OnTrack TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMeasurementBase TrkPseudoMeasurementOnTrack TrkFitterInterfaces )

# Install files from the package:
atlas_install_headers( InDetTrackSplitterTool )

