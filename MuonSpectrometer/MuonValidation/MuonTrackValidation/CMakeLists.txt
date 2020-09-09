################################################################################
# Package: MuonTrackValidation
################################################################################

# Declare the package name:
atlas_subdir( MuonTrackValidation )

# External dependencies:
find_package( ROOT COMPONENTS Core Tree MathCore Hist RIO pthread )

# Component(s) in the package:
atlas_add_component( MuonTrackValidation
                     src/*.cxx
                     src/components/*.cxx
                     INCLUDE_DIRS ${ROOT_INCLUDE_DIRS}
                     LINK_LIBRARIES ${ROOT_LIBRARIES} AthenaBaseComps GaudiKernel MuonIdHelpersLib TrkEventPrimitives TrkEventUtils TrkValInterfaces )

