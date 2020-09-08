################################################################################
# Package: MuidCaloIsolationTools
################################################################################

# Declare the package name:
atlas_subdir( MuidCaloIsolationTools )

# Component(s) in the package:
atlas_add_component( MuidCaloIsolationTools
                     src/MuidTrackIsolation.cxx
                     src/components/*.cxx
                     LINK_LIBRARIES AthenaBaseComps GaudiKernel MuidInterfaces TrkTrack TrkSurfaces TrkParameters TrkExInterfaces TrkExUtils )

# Install files from the package:
atlas_install_headers( MuidCaloIsolationTools )

