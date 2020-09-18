################################################################################
# Package: MuonResidualPullCalculators
################################################################################

# Declare the package name:
atlas_subdir( MuonResidualPullCalculators )

# Component(s) in the package:
atlas_add_component( MuonResidualPullCalculators
                     src/*.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps EventPrimitives GaudiKernel MuonReadoutGeometry MuonIdHelpersLib MuonCompetingRIOsOnTrack TrkEventPrimitives TrkEventUtils TrkParameters TrkRIO_OnTrack TrkToolInterfaces )
