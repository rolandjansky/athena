################################################################################
# Package: BCM_CollisionTime
################################################################################

# Declare the package name:
atlas_subdir( BCM_CollisionTime )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Control/CLIDSvc
                          Control/SGTools
                          PRIVATE
                          GaudiKernel )

# Component(s) in the package:
atlas_add_component( BCM_CollisionTime
                     src/components/*.cxx
                     LINK_LIBRARIES SGTools GaudiKernel )

# Install files from the package:
atlas_install_headers( BCM_CollisionTime )

