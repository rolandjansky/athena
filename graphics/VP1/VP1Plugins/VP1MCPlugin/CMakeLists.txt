################################################################################
# Package: VP1MCPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1MCPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Systems/VP1MCSystems )

# Install files from the package:
atlas_install_headers( VP1MCPlugin )

