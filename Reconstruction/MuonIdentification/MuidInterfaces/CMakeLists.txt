################################################################################
# Package: MuidInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MuidInterfaces )

# Component(s) in the package:
atlas_add_library( MuidInterfaces
                   PUBLIC_HEADERS MuidInterfaces
                   LINK_LIBRARIES GaudiKernel MuidEvent TrkEventPrimitives TrkParameters TrkTrack TrkFitterInterfaces TrkFitterUtils )

