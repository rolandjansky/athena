################################################################################
# Package: EventShapeAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( EventShapeAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Database/AthenaPOOL/AthenaPoolUtilities
                          PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Reconstruction/EventShapes/EventShapeEvent
                          Reconstruction/EventShapes/EventShapeTPCnv )

# Component(s) in the package:
atlas_add_poolcnv_library( EventShapeAthenaPoolPoolCnv
                           src/*.cxx
                           FILES EventShapeEvent/EventShapeStore.h
                           TYPES_WITH_NAMESPACE 
                           LINK_LIBRARIES AthenaPoolUtilities AthenaPoolCnvSvcLib EventShapeEvent EventShapeTPCnv )

# Install files from the package:
atlas_install_headers( EventShapeAthenaPool )

