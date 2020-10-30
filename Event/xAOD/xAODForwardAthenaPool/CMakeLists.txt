# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODForwardAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODForwardAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODForward/ALFADataContainer.h
   xAODForward/ALFADataAuxContainer.h
   xAODForward/AFPDataContainer.h
   xAODForward/AFPDataAuxContainer.h
   xAODForward/AFPSiHitContainer.h
   xAODForward/AFPSiHitAuxContainer.h
   xAODForward/AFPToFHitContainer.h
   xAODForward/AFPToFHitAuxContainer.h
   xAODForward/AFPSiHitsClusterContainer.h
   xAODForward/AFPSiHitsClusterAuxContainer.h
   xAODForward/AFPTrackContainer.h
   xAODForward/AFPTrackAuxContainer.h
   xAODForward/AFPProtonContainer.h
   xAODForward/AFPProtonAuxContainer.h
   xAODForward/ZdcModuleContainer.h
   xAODForward/ZdcModuleAuxContainer.h
   xAODForward/MBTSModuleContainer.h
   xAODForward/MBTSModuleAuxContainer.h
   xAODForward/ForwardEventInfoContainer.h
   xAODForward/ForwardEventInfoAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::ALFADataContainer xAOD::ALFADataAuxContainer
   xAOD::AFPDataContainer xAOD::AFPDataAuxContainer
   xAOD::AFPSiHitContainer xAOD::AFPSiHitAuxContainer
   xAOD::AFPToFHitContainer xAOD::AFPToFHitAuxContainer
   xAOD::AFPSiHitsClusterContainer xAOD::AFPSiHitsClusterAuxContainer
   xAOD::AFPTrackContainer xAOD::AFPTrackAuxContainer
   xAOD::AFPProtonContainer xAOD::AFPProtonAuxContainer
   xAOD::ZdcModuleContainer xAOD::ZdcModuleAuxContainer
   xAOD::MBTSModuleContainer xAOD::MBTSModuleAuxContainer
   xAOD::ForwardEventInfoContainer xAOD::ForwardEventInfoAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthContainers AthenaKernel AthenaPoolCnvSvcLib
   AthenaPoolUtilities xAODForward )
