################################################################################
# Package: xAODTrigEgammaAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigEgammaAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEgamma
                          Event/xAOD/xAODTrigEgamma
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigEgammaAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigEgamma/TrigPhotonContainer.h xAODTrigEgamma/TrigPhotonAuxContainer.h xAODTrigEgamma/TrigElectronContainer.h xAODTrigEgamma/TrigElectronAuxContainer.h xAODTrigEgamma/ElectronTrigAuxContainer.h xAODTrigEgamma/PhotonTrigAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigPhotonContainer xAOD::TrigElectronContainer xAOD::ElectronTrigAuxContainer xAOD::PhotonTrigAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEgamma xAODTrigEgamma GaudiKernel )

