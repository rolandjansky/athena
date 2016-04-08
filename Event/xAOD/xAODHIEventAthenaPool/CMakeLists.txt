################################################################################
# Package: xAODHIEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODHIEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODHIEvent )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODHIEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODHIEvent/HIEventShapeContainer.h xAODHIEvent/HIEventShapeAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::HIEventShapeContainer xAOD::HIEventShapeAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODHIEvent )

