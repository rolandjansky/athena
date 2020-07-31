# Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration

# Declare the package name:
atlas_subdir( TrigBphysicsEvent )

# External dependencies:
find_package( CLHEP )

# Component(s) in the package:
atlas_add_library( TrigBphysicsEvent
                   src/*.cxx
                   PUBLIC_HEADERS TrigBphysicsEvent
                   INCLUDE_DIRS ${CLHEP_INCLUDE_DIRS}
                   DEFINITIONS ${CLHEP_DEFINITIONS}
                   LINK_LIBRARIES ${CLHEP_LIBRARIES} AthContainers AthenaKernel TrigSerializeCnvSvcLib
                   PRIVATE_LINK_LIBRARIES TrigInDetEvent TrigMuonEvent TrigNavigationLib )

atlas_add_sercnv_library( TrigBphysicsEventxAODSerCnv
                          FILES xAODTrigBphys/TrigBphysContainer.h xAODTrigBphys/TrigBphysAuxContainer.h
                          TYPES_WITH_NAMESPACE xAOD::TrigBphysContainer xAOD::TrigBphysAuxContainer
                          CNV_PFX xAOD
                          LINK_LIBRARIES xAODTrigBphys TrigBphysicsEvent )

atlas_add_dictionary( TrigBphysicsEventDict
                      TrigBphysicsEvent/TrigBphysicsEventDict.h
                      TrigBphysicsEvent/selection.xml
                      LINK_LIBRARIES TrigBphysicsEvent )
