################################################################################
# Package: TrkRefitAlg
################################################################################

# Declare the package name:
atlas_subdir( TrkRefitAlg )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkFitter/TrkFitterUtils
                          InnerDetector/InDetConditions/BeamSpotConditionsData
                          InnerDetector/InDetDetDescr/InDetReadoutGeometry
                          PRIVATE
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrackSummary
                          Tracking/TrkEvent/TrkVertexOnTrack
                          Tracking/TrkEvent/VxVertex
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( TrkRefitAlg
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkEventPrimitives TrkEventUtils TrkTrack TrkFitterUtils InDetReadoutGeometry TrkSurfaces TrkMeasurementBase TrkParameters TrkTrackSummary TrkVertexOnTrack VxVertex TrkExInterfaces TrkFitterInterfaces TrkToolInterfaces BeamSpotConditionsData )

# Install files from the package:
atlas_install_headers( TrkRefitAlg )

