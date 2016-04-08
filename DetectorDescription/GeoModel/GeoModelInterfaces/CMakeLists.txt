################################################################################
# Package: GeoModelInterfaces
################################################################################

# Declare the package name:
atlas_subdir( GeoModelInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          Control/CLIDSvc
                          Control/SGTools
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( GeoModelInterfaces )

