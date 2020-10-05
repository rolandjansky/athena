# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigMinBiasAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigMinBiasAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigMinBias/TrigSpacePointCountsContainer.h xAODTrigMinBias/TrigSpacePointCountsAuxContainer.h xAODTrigMinBias/TrigVertexCountsContainer.h xAODTrigMinBias/TrigVertexCountsAuxContainer.h xAODTrigMinBias/TrigTrackCountsContainer.h xAODTrigMinBias/TrigTrackCountsAuxContainer.h xAODTrigMinBias/TrigT2MbtsBitsContainer.h xAODTrigMinBias/TrigT2MbtsBitsAuxContainer.h xAODTrigMinBias/TrigT2ZdcSignalsContainer.h xAODTrigMinBias/TrigT2ZdcSignalsAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigSpacePointCountsContainer xAOD::TrigSpacePointCountsAuxContainer xAOD::TrigVertexCountsContainer xAOD::TrigVertexCountsAuxContainer xAOD::TrigTrackCountsContainer xAOD::TrigTrackCountsAuxContainer xAOD::TrigT2MbtsBitsContainer xAOD::TrigT2MbtsBitsAuxContainer xAOD::TrigT2ZdcSignalsContainer xAOD::TrigT2ZdcSignalsAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities xAODTrigMinBias )

