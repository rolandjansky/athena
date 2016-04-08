################################################################################
# Package: VP1AODPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP1AODPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          graphics/VP1/VP1Base
                          PRIVATE
                          graphics/VP1/VP1Systems/VP1AODSystems
                          graphics/VP1/VP1Systems/VP1GuideLineSystems )

# Install files from the package:
atlas_install_headers( VP1AODPlugin )

