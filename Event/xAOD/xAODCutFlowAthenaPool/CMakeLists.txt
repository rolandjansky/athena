################################################################################
# Package: xAODCutFlowAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODCutFlowAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Control/AthContainers
                          Control/AthenaKernel
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODCutFlow )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCutFlowAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODCutFlow/CutBookkeeperContainer.h xAODCutFlow/CutBookkeeperAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::CutBookkeeperContainer xAOD::CutBookkeeperAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCutFlow )

