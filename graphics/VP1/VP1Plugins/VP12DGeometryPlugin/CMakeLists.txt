################################################################################
# Package: VP12DGeometryPlugin
################################################################################

# Declare the package name:
atlas_subdir( VP12DGeometryPlugin )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          graphics/VP1/VP1Systems/VP12DGeometrySystems )

# Install files from the package:
atlas_install_headers( VP12DGeometryPlugin )

