################################################################################
# Package: FastSimulationEvent
################################################################################

# Declare the package name:
atlas_subdir( FastSimulationEvent )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          DetectorDescription/GeoPrimitives
                          Tracking/TrkEvent/TrkMaterialOnTrack )

# External dependencies:
find_package( Eigen )

# Install files from the package:
atlas_install_headers( FastSimulationEvent )

