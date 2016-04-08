################################################################################
# Package: xAODTrigMinBiasAthenaPool
################################################################################

# Declare the package name:
atlas_subdir( xAODTrigMinBiasAthenaPool )

# Declare the package's dependencies:
atlas_depends_on_subdirs( PRIVATE
                          Database/AthenaPOOL/AthenaPoolCnvSvc
                          Database/AthenaPOOL/AthenaPoolUtilities
                          Event/xAOD/xAODTrigMinBias )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMinBiasAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigMinBias/TrigSpacePointCountsContainer.h xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h xAODTrigMinBias/TrigVertexCountsContainer.h xAODTrigMinBias/TrigVertexCountsAuxContainer.h xAODTrigMinBias/TrigTrackCountsContainer.h xAODTrigMinBias/TrigTrackCountsAuxContainer.h xAODTrigMinBias/TrigT2MbtsBitsContainer.h xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h xAODTrigMinBias/TrigT2ZdcSignalsContainer.h xAODTrigMinBias/TrigT2ZdcSignalsAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigSpacePointCountsContainer xAOD::TrigSpacePointCountsAuxContainer.h xAOD::TrigVertexCountsContainer xAOD::TrigVertexCountsAuxContainer xAOD::TrigTrackCountsContainer xAOD::TrigTrackCountsAuxContainer xAOD::TrigT2MbtsBitsContainer xAOD::TrigT2MbtsBitsAuxContainer xAOD::TrigT2ZdcSignalsContainer xAOD::TrigT2ZdcSignalsAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigMinBias )

