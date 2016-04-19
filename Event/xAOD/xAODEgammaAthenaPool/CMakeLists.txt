################################################################################
# Package: xAODEgammaAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODEgammaAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODEgamma
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEgammaAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODEgamma/ElectronContainer.h xAODEgamma/ElectronAuxContainer.h xAODEgamma/PhotonContainer.h xAODEgamma/PhotonAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::ElectronContainer xAOD::ElectronAuxContainer xAOD::PhotonContainer xAOD::PhotonAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODEgamma GaudiKernel )

