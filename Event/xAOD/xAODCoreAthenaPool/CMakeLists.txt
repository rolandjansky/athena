# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODCoreAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODCoreAthenaPoolPoolCnv
   src/*.cxx
   FILES xAODCore/AuxInfoBase.h xAODCore/AuxContainerBase.h
   xAODCore/ShallowAuxContainer.h xAODCore/ShallowAuxInfo.h
   TYPES_WITH_NAMESPACE xAOD::AuxInfoBase xAOD::AuxContainerBase
   xAOD::ShallowAuxContainer xAOD::ShallowAuxInfo
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib xAODCore )
