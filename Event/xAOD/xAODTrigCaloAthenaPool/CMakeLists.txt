################################################################################
# Package: xAODTrigCaloAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigCaloAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODCaloEvent
                          Event/xAOD/xAODTrigCalo
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigCaloAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigCalo/TrigCaloClusterContainer.h xAODTrigCalo/TrigCaloClusterAuxContainer.h xAODTrigCalo/TrigEMClusterContainer.h xAODTrigCalo/TrigEMClusterAuxContainer.h xAODTrigCalo/CaloClusterTrigAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigCaloClusterContainer xAOD::TrigCaloClusterAuxContainer xAOD::TrigEMClusterContainer xAOD::TrigEMClusterAuxContainer xAOD::CaloClusterTrigAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCaloEvent xAODTrigCalo GaudiKernel )

