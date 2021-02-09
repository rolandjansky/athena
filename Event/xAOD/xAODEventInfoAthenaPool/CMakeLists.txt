# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( xAODEventInfoAthenaPool )

# Component(s) in the package:
atlas_add_poolcnv_library( xAODEventInfoAthenaPoolPoolCnv
   src/*.h src/*.cxx
   FILES xAODEventInfo/EventInfo.h xAODEventInfo/EventAuxInfo.h
      xAODEventInfo/EventInfoContainer.h xAODEventInfo/EventInfoAuxContainer.h
   TYPES_WITH_NAMESPACE xAOD::EventInfo xAOD::EventAuxInfo
      xAOD::EventInfoContainer xAOD::EventInfoAuxContainer
   CNV_PFX xAOD
   LINK_LIBRARIES AthenaPoolCnvSvcLib AthenaPoolUtilities AthContainers
      xAODEventInfo )
