################################################################################
# Package: xAODCaloEventAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODCaloEventAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PUBLIC
                          Calorimeter/CaloInterface
                          PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODCaloEvent
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCaloEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODCaloEvent/CaloClusterContainer.h xAODCaloEvent/CaloClusterAuxContainer.h xAODCaloEvent/CaloTowerContainer.h xAODCaloEvent/CaloTowerAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::CaloClusterContainer xAOD::CaloClusterAuxContainer xAOD::CaloTowerContainer xAOD::CaloTowerAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCaloEvent GaudiKernel )

