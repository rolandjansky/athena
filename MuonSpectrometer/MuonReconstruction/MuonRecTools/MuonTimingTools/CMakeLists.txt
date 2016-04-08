################################################################################
# Package: MuonTimingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTimingTools )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthenaBaseComps
                          GaudiKernel
                          MuonSpectrometer/MuonIdHelpers
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonCompetingRIOsOnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecEvent/MuonRIO_OnTrack
                          MuonSpectrometer/MuonReconstruction/MuonRecTools/MuonRecToolInterfaces )

# Component(s) in the package:
atlas_add_component( MuonTimingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonRecToolInterfaces )

