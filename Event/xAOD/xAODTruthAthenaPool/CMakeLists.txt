################################################################################
# Package: xAODTruthAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTruthAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTruth )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTruthAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTruth/TruthParticleContainer.h xAODTruth/TruthParticleAuxContainer.h xAODTruth/TruthVertexContainer.h xAODTruth/TruthVertexAuxContainer.h xAODTruth/TruthEventContainer.h xAODTruth/TruthEventAuxContainer.h xAODTruth/TruthPileupEventContainer.h xAODTruth/TruthPileupEventAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TruthParticleContainer xAOD::TruthParticleAuxContainer xAOD::TruthVertexContainer xAOD::TruthVertexAuxContainer xAOD::TruthEventContainer xAOD::TruthEventAuxContainer xAOD::TruthPileupEventContainer xAOD::TruthPileupEventAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTruth )

