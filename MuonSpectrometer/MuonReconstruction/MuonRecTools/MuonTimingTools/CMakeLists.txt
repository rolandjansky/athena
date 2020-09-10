################################################################################
# Package: MuonTimingTools
################################################################################

# Declare the package name:
atlas_subdir( MuonTimingTools )

# Component(s) in the package:
atlas_add_component( MuonTimingTools
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuonIdHelpersLib MuonCompetingRIOsOnTrack MuonRIO_OnTrack MuonRecToolInterfaces )

