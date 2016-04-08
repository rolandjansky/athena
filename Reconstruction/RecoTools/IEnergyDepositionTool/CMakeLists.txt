################################################################################
# Package: IEnergyDepositionTool
################################################################################

# Declare the package name:
atlas_subdir( IEnergyDepositionTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloIdentifier
                          GaudiKernel
                          Tracking/TrkEvent/TrkEventPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack
                          Tracking/TrkEvent/TrkParameters )

# Install files from the package:
atlas_install_headers( IEnergyDepositionTool )

