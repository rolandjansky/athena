################################################################################
# Package: xAODTauAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTauAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
			  Control/StoreGate
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
			  Event/xAOD/xAODTracking
                          Event/xAOD/xAODTau
                          GaudiKernel )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTauAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTau/TauJetContainer.h xAODTau/TauJetAuxContainer.h xAODTau/DiTauJetContainer.h xAODTau/DiTauJetAuxContainer.h xAODTau/TauTrackContainer.h xAODTau/TauTrackAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TauJetContainer xAOD::TauJetAuxContainer xAOD::DiTauJetContainer xAOD::DiTauJetAuxContainer xAOD::TauTrackContainer xAOD::TauTrackAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel StoreGateLib AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTracking xAODTau GaudiKernel )

