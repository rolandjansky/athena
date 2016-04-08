################################################################################
# Package: xAODTrigMissingETAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMissingETAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigMissingET
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMissingETAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigMissingET/TrigMissingETContainer.h xAODTrigMissingET/TrigMissingETAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigMissingET xAOD::TrigMissingETComponent
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigMissingET GaudiKernel )

