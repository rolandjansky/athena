################################################################################
# Package: TrkDeterministicAnnealingFilter
################################################################################

# Declare the package name:
atlas_subdir( TrkDeterministicAnnealingFilter )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaBaseComps
                          GaudiKernel
                          Tracking/TrkEvent/TrkCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils
                          Tracking/TrkTools/TrkToolInterfaces
                          PRIVATE
                          DetectorDescription/AtlasDetDescr
                          Tracking/TrkDetDescr/TrkSurfaces
                          Tracking/TrkEvent/TrkMeasurementBase
                          Tracking/TrkEvent/TrkPrepRawData
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkExtrapolation/TrkExInterfaces
                          Tracking/TrkValidation/TrkValInterfaces )

# Component(s) in the package:
atlas_add_component( TrkDeterministicAnnealingFilter
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel TrkCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkFitterInterfaces TrkFitterUtils TrkToolInterfaces AtlasDetDescr TrkSurfaces TrkMeasurementBase TrkPrepRawData TrkRIO_OnTrack TrkTrack TrkExInterfaces TrkValInterfaces )

# Install files from the package:
atlas_install_headers( TrkDeterministicAnnealingFilter )

