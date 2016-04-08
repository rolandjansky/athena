################################################################################
# Package: TrigSiSpacePointTool
################################################################################

# Declare the package name:
atlas_subdir( TrigSiSpacePointTool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          GaudiKernel
                          Trigger/TrigEvent/TrigInDetEvent )

# Install files from the package:
atlas_install_headers( TrigSiSpacePointTool )

