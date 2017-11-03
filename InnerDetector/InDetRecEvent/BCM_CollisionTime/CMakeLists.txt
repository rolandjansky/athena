################################################################################
# Package: BCM_CollisionTime
################################################################################

# Declare the package name:
atlas_subdir( BCM_CollisionTime )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/AthenaKernel
                          PRIVATE
                          GaudiKernel )

# Install files from the package:
atlas_install_headers( BCM_CollisionTime )

