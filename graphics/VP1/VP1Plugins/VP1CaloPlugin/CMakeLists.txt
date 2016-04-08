################################################################################
# Package: VP1CaloPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1CaloPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Systems/VP1CaloClusterSystems
                          graphics/VP1/VP1Systems/VP1CaloReadoutSystems
                          graphics/VP1/VP1Systems/VP1CaloSystems
                          graphics/VP1/VP1Systems/VP1GeometrySystems
                          graphics/VP1/VP1Systems/VP1GuideLineSystems )

# Install files from the package:
atlas_install_headers( VP1CaloPlugin )

