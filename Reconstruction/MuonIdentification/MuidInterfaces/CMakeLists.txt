################################################################################
# Package: MuidInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuidInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Reconstruction/MuonIdentification/MuidEvent
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkParameters
                          Tracking/TrkEvent/TrkTrack
                          Tracking/TrkFitter/TrkFitterInterfaces
                          Tracking/TrkFitter/TrkFitterUtils )

# Component(s) in the package:
atlas_add_library( MuidInterfaces
                   PUBLIC_HEADERS MuidInterfaces
                   LINK_LIBRARIES GaudiKernel MuidEvent TrkEventPrimitives TrkParameters TrkTrack TrkFitterInterfaces TrkFitterUtils )

