################################################################################
# Package: MdtCalibInterfaces
################################################################################

# Declare the package name:
atlas_subdir( MdtCalibInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( MdtCalibInterfaces )

