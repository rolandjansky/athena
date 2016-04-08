################################################################################
# Package: MuonResidualPullCalculators
################################################################################

# Declare the package name:
atlas_subdir( MuonResidualPullCalculators )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          Event/EventPrimitives
                          GaudiKernel
                          MuonSpectrometer/MuonDetDescr/MuonReadoutGeometry
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkEventUtils
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkRIO_OnTrack
                          Tracking/TrkTools/TrkToolInterfaces )

# Component(s) in the package:
atlas_add_component( MuonResidualPullCalculators
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkRIO_OnTrack TrkToolInterfaces )

# Install files from the package:
atlas_install_headers( MuonResidualPullCalculators )

