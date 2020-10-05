# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigMissingEtEvent )

# Component(s) in the package:
atlas_add_library( TrigMissingEtEvent
                   src/*.cxx
                   src/components/*.cxx
                   PUBLIC_HEADERS TrigMissingEtEvent
                   PRIVATE_LINK_LIBRARIES TrigNavigationLib xAODTrigMissingET
                   LINK_LIBRARIES AthContainers AthenaKernel GaudiKernel TrigSerializeCnvSvcLib )


atlas_add_sercnv_library( TrigMissingEtEventSerCnv
                          FILES TrigMissingEtEvent/TrigMissingET.h TrigMissingEtEvent/TrigMissingETContainer.h TrigMissingEtEvent/TrigMissingEtComponent.h
                          LINK_LIBRARIES TrigMissingEtEvent )

atlas_add_sercnv_library( TrigMissingEtEventxAODSerCnv
                          FILES xAODTrigMissingET/TrigMissingETContainer.h xAODTrigMissingET/TrigMissingETAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TrigMissingETContainer xAOD::TrigMissingETAuxContainer
                          CNV_PFX xAOD
                          LINK_LIBRARIES TrigMissingEtEvent xAODTrigMissingET )

atlas_add_dictionary( TrigMissingEtEventDict
                      TrigMissingEtEvent/TrigMissingEtEventDict.h
                      TrigMissingEtEvent/selection.xml
                      LINK_LIBRARIES TrigMissingEtEvent
                      DATA_LINKS TrigMissingET )
