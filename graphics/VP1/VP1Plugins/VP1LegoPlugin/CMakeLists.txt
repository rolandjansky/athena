################################################################################
# Package: VP1LegoPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1LegoPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Systems/VP1LegoSystems )

# Install files from the package:
atlas_install_headers( VP1LegoPlugin )

