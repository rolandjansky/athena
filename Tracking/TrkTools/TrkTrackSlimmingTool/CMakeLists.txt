################################################################################
# Package: TrkTrackSlimmingTool
################################################################################

# Declare the package name:
atlas_subdir( TrkTrackSlimmingTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          GaudiKernel
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkPseudoMeasurementOnTrack
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkEvent/TrkTrackSummary )

# Component(s) in the package:
atlas_add_component( TrkTrackSlimmingTool
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps TrkToolInterfaces AtlasDetDescr GaudiKernel TrkSurfaces TrkCompetingRIOsOnTrack TrkEventPrimitives TrkMaterialOnTrack TrkMeasurementBase TrkParameters TrkPseudoMeasurementOnTrack TrkRIO_OnTrack TrkTrack TrkTrackSummary )

# Install files from the package:
atlas_install_headers( TrkTrackSlimmingTool )

