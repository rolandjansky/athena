# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODTrigBphysAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODTrigBphysAthenaPoolPoolCnv
                           src/*.cxx
                           FILES xAODTrigBphys/TrigBphysContainer.h xAODTrigBphys/TrigBphysAuxContainer.h
                           TYPES_WITH_NAMESPACE xAOD::TrigBphysContainer xAOD::TrigBphysAuxContainer
                           CNV_PFX xAOD
                           LINK_LIBRARIES AthenaPoolCnvSvcLib xAODTrigBphys )
