# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODCutFlowAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCutFlowAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODCutFlow/CutBookkeeperContainer.h xAODCutFlow/CutBookkeeperAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::CutBookkeeperContainer xAOD::CutBookkeeperAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODCutFlow )

