################################################################################
# Package: xAODTrigMuonAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMuonAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Control/CxxUtils
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigMuon )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMuonAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigMuon/L2StandAloneMuonContainer.h xAODTrigMuon/L2StandAloneMuonAuxContainer.h xAODTrigMuon/L2IsoMuonContainer.h xAODTrigMuon/L2IsoMuonAuxContainer.h xAODTrigMuon/L2CombinedMuonContainer.h xAODTrigMuon/L2CombinedMuonAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::L2StandAloneMuonContainer xAOD::L2StandAloneMuonAuxContainer xAOD::L2IsoMuonContainer xAOD::L2IsoMuonAuxContainer xAOD::L2CombinedMuonContainer xAOD::L2CombinedMuonAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel CxxUtils AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigMuon )

