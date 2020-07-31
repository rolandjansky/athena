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

atlas_add_library( BCM_CollisionTimeLib
                   BCM_CollisionTime/*.h
                   INTERFACE
                   PUBLIC_HEADERS BCM_CollisionTime
                   LINK_LIBRARIES AthenaKernel )


