# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODHIEventAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODHIEventAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODHIEvent/HIEventShapeContainer.h xAODHIEvent/HIEventShapeAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::HIEventShapeContainer xAOD::HIEventShapeAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib AthenaPoolUtilities xAODHIEvent )

