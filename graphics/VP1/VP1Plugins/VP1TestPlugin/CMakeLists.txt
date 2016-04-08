################################################################################
# Package: VP1TestPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1TestPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          graphics/VP1/VP1Systems/VP1GeometrySystems
                          graphics/VP1/VP1Systems/VP1TestSystems )

# Install files from the package:
atlas_install_headers( VP1TestPlugin )

