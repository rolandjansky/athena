################################################################################
# Package: xAODCaloRingsAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloRingsAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODCaloRings )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCaloRingsAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODCaloRings/CaloRingsContainer.h xAODCaloRings/CaloRingsAuxContainer.h xAODCaloRings/RingSetContainer.h xAODCaloRings/RingSetAuxContainer.h xAODCaloRings/RingSetConfContainer.h xAODCaloRings/RingSetConfAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::CaloRingsContainer xAOD::CaloRingsAuxContainer xAOD::RingSetContainer xAOD::RingSetAuxContainer xAOD::RingSetConfContainer xAOD::RingSetConfAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCaloRings )

