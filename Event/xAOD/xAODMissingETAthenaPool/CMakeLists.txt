################################################################################
# Package: xAODMissingETAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODMissingETAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODMissingET
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODMissingETAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODMissingET/MissingETContainer.h xAODMissingET/MissingETAuxContainer.h xAODMissingET/MissingETComponentMap.h xAODMissingET/MissingETAuxComponentMap.h xAODMissingET/MissingETAssociationMap.h xAODMissingET/MissingETAuxAssociationMap.h
                           TYPES_WITH_NAMESPACE xAOD::MissingETContainer xAOD::MissingETAuxContainer xAOD::MissingETComponentMap xAOD::MissingETAuxComponentMap xAOD::MissingETAssociationMap xAOD::MissingETAuxAssociationMap
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODMissingET GaudiKernel )

