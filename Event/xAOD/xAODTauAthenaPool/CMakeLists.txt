################################################################################
# Package: xAODTauAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTauAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTau
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTauAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTau/TauJetContainer.h xAODTau/TauJetAuxContainer.h xAODTau/DiTauJetContainer.h xAODTau/DiTauJetAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TauJetContainer xAOD::TauJetAuxContainer xAOD::DiTauJetContainer xAOD::DiTauJetAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTau GaudiKernel )

