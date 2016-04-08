################################################################################
# Package: xAODEventShapeAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODEventShapeAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEventShape )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEventShapeAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODEventShape/EventShape.h xAODEventShape/EventShapeAuxInfo.h
                           TYPES_WITH_NAMESPACE xAOD::EventShape xAOD::EventShapeAuxInfo
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEventShape )

