// Dear emacs, this is -*- c++ -*-
/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/
#ifndef XAODTRIGGER_XAODTRIGGERDICT_H
#define XAODTRIGGER_XAODTRIGGERDICT_H

// Local include(s).
#include "xAODTrigger/versions/ByteStreamAuxContainer_v1.h"

#include "xAODTrigger/TriggerMenu.h"
#include "xAODTrigger/TriggerMenuContainer.h"
#include "xAODTrigger/TriggerMenuAuxContainer.h"
#include "xAODTrigger/versions/TriggerMenu_v1.h"
#include "xAODTrigger/versions/TriggerMenuContainer_v1.h"
#include "xAODTrigger/versions/TriggerMenuAuxContainer_v1.h"

#include "xAODTrigger/TriggerMenuJson.h"
#include "xAODTrigger/TriggerMenuJsonContainer.h"
#include "xAODTrigger/TriggerMenuJsonAuxContainer.h"
#include "xAODTrigger/versions/TriggerMenuJson_v1.h"
#include "xAODTrigger/versions/TriggerMenuJsonContainer_v1.h"
#include "xAODTrigger/versions/TriggerMenuJsonAuxContainer_v1.h"

#include "xAODTrigger/MuonRoI.h"
#include "xAODTrigger/MuonRoIContainer.h"
#include "xAODTrigger/MuonRoIAuxContainer.h"
#include "xAODTrigger/versions/MuonRoI_v1.h"
#include "xAODTrigger/versions/MuonRoIContainer_v1.h"
#include "xAODTrigger/versions/MuonRoIAuxContainer_v1.h"

#include "xAODTrigger/JetRoI.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODTrigger/JetRoIAuxContainer.h"
#include "xAODTrigger/versions/JetRoI_v1.h"
#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/versions/JetRoIAuxContainer_v1.h"
#include "xAODTrigger/versions/JetRoI_v2.h"
#include "xAODTrigger/versions/JetRoIContainer_v2.h"
#include "xAODTrigger/versions/JetRoIAuxContainer_v2.h"

#include "xAODTrigger/EmTauRoI.h"
#include "xAODTrigger/EmTauRoIContainer.h"
#include "xAODTrigger/EmTauRoIAuxContainer.h"
#include "xAODTrigger/versions/EmTauRoI_v1.h"
#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoI_v2.h"
#include "xAODTrigger/versions/EmTauRoIContainer_v2.h"
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v2.h"

#include "xAODTrigger/JetEtRoI.h"
#include "xAODTrigger/JetEtRoIAuxInfo.h"
#include "xAODTrigger/versions/JetEtRoI_v1.h"
#include "xAODTrigger/versions/JetEtRoIAuxInfo_v1.h"

#include "xAODTrigger/EnergySumRoI.h"
#include "xAODTrigger/EnergySumRoIAuxInfo.h"
#include "xAODTrigger/versions/EnergySumRoI_v1.h"
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v1.h"
#include "xAODTrigger/versions/EnergySumRoI_v2.h"
#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v2.h"

#include "xAODTrigger/eFexEMRoI.h"
#include "xAODTrigger/eFexEMRoIContainer.h"
#include "xAODTrigger/eFexEMRoIAuxContainer.h"
#include "xAODTrigger/versions/eFexEMRoI_v1.h"
#include "xAODTrigger/versions/eFexEMRoIContainer_v1.h"
#include "xAODTrigger/versions/eFexEMRoIAuxContainer_v1.h"

#include "xAODTrigger/TrigDecision.h"
#include "xAODTrigger/TrigDecisionAuxInfo.h"
#include "xAODTrigger/versions/TrigDecision_v1.h"
#include "xAODTrigger/versions/TrigDecisionAuxInfo_v1.h"

#include "xAODTrigger/TrigNavigation.h"
#include "xAODTrigger/TrigNavigationAuxInfo.h"
#include "xAODTrigger/versions/TrigNavigation_v1.h"
#include "xAODTrigger/versions/TrigNavigationAuxInfo_v1.h"

#include "xAODTrigger/TrigConfKeys.h"
#include "xAODTrigger/versions/TrigConfKeys_v1.h"

#include "xAODTrigger/TrigComposite.h"
#include "xAODTrigger/TrigCompositeContainer.h"
#include "xAODTrigger/TrigCompositeAuxContainer.h"
#include "xAODTrigger/versions/TrigComposite_v1.h"
#include "xAODTrigger/versions/TrigCompositeContainer_v1.h"
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v2.h"

#include "xAODTrigger/BunchConfKey.h"
#include "xAODTrigger/versions/BunchConfKey_v1.h"

#include "xAODTrigger/BunchConf.h"
#include "xAODTrigger/BunchConfContainer.h"
#include "xAODTrigger/BunchConfAuxContainer.h"
#include "xAODTrigger/versions/BunchConf_v1.h"
#include "xAODTrigger/versions/BunchConfContainer_v1.h"
#include "xAODTrigger/versions/BunchConfAuxContainer_v1.h"

#include "xAODTrigger/TrigPassBits.h"
#include "xAODTrigger/TrigPassBitsContainer.h"
#include "xAODTrigger/TrigPassBitsAuxContainer.h"
#include "xAODTrigger/versions/TrigPassBits_v1.h"
#include "xAODTrigger/versions/TrigPassBitsContainer_v1.h"
#include "xAODTrigger/versions/TrigPassBitsAuxContainer_v1.h"

#include "xAODTrigger/RoiDescriptorStore.h"
#include "xAODTrigger/RoiDescriptorStoreAuxInfo.h"
#include "xAODTrigger/versions/RoiDescriptorStore_v1.h"
#include "xAODTrigger/versions/RoiDescriptorStoreAuxInfo_v1.h"

// EDM include(s).
#include "xAODCore/tools/DictHelpers.h"

// Instantiate all necessary types for the dictionary.
namespace {
   struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGGER {
      // Local type(s).
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TriggerMenuContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TriggerMenuJsonContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, BunchConfContainer_v1 );

      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, MuonRoIContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JetRoIContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, JetRoIContainer_v2 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, EmTauRoIContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, EmTauRoIContainer_v2 );

      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, eFexEMRoIContainer_v1 );

      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigCompositeContainer_v1 );
      XAOD_INSTANTIATE_NS_CONTAINER_TYPES( xAOD, TrigPassBitsContainer_v1 );

      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, JetEtRoI_v1 );
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, EnergySumRoI_v1 );
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, EnergySumRoI_v2 );
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, TrigDecision_v1 );
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, TrigNavigation_v1 );
      XAOD_INSTANTIATE_NS_OBJECT_TYPES( xAOD, RoiDescriptorStore_v1 );
   };
}

#endif // XAODTRIGGER_XAODTRIGGERDICT_H
