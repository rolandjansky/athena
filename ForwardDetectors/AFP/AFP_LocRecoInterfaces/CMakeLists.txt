################################################################################
# Package: AFP_LocRecoInterfaces
################################################################################

# Declare the package name:
atlas_subdir( AFP_LocRecoInterfaces )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthContainers
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( AFP_LocRecoInterfaces )

