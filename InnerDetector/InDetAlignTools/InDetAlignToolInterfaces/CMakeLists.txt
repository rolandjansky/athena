################################################################################
# Package: InDetAlignToolInterfaces
################################################################################

# Declare the package name:
atlas_subdir( InDetAlignToolInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/DataModel
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( InDetAlignToolInterfaces )

