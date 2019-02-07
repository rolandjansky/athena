// Dear emacs, this is -*- c++ -*-

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: xAODTriggerDict.h 743093 2016-04-26 21:46:56Z watsona $
#ifndef XAODTRIGGER_XAODTRIGGERDICT_H
#define XAODTRIGGER_XAODTRIGGERDICT_H

// STL include(s):
#include <vector>

// EDM include(s):
#include "AthLinks/DataLink.h"
#include "AthLinks/ElementLink.h"
#include "AthLinks/ElementLinkVector.h"

// Local include(s):
#include "xAODTrigger/versions/ByteStreamAuxContainer_v1.h"

#include "xAODTrigger/versions/TriggerMenu_v1.h"
#include "xAODTrigger/versions/TriggerMenuContainer_v1.h"
#include "xAODTrigger/versions/TriggerMenuAuxContainer_v1.h"

#include "xAODTrigger/versions/MuonRoIContainer_v1.h"
#include "xAODTrigger/versions/MuonRoIAuxContainer_v1.h"
#include "xAODTrigger/versions/MuonRoI_v1.h"

#include "xAODTrigger/versions/JetRoIContainer_v1.h"
#include "xAODTrigger/versions/JetRoIAuxContainer_v1.h"
#include "xAODTrigger/versions/JetRoI_v1.h"

#include "xAODTrigger/versions/JetRoIContainer_v2.h"
#include "xAODTrigger/versions/JetRoIAuxContainer_v2.h"
#include "xAODTrigger/versions/JetRoI_v2.h"

#include "xAODTrigger/versions/EmTauRoIContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v1.h"
#include "xAODTrigger/versions/EmTauRoI_v1.h"

#include "xAODTrigger/versions/EmTauRoIContainer_v2.h"
#include "xAODTrigger/versions/EmTauRoIAuxContainer_v2.h"
#include "xAODTrigger/versions/EmTauRoI_v2.h"

#include "xAODTrigger/versions/JetEtRoIAuxInfo_v1.h"
#include "xAODTrigger/versions/JetEtRoI_v1.h"

#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v2.h"
#include "xAODTrigger/versions/EnergySumRoI_v2.h"

#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v1.h"
#include "xAODTrigger/versions/EnergySumRoI_v1.h"

#include "xAODTrigger/versions/EnergySumRoIAuxInfo_v2.h"
#include "xAODTrigger/versions/EnergySumRoI_v2.h"

#include "xAODTrigger/versions/TrigDecision_v1.h"
#include "xAODTrigger/versions/TrigDecisionAuxInfo_v1.h"

#include "xAODTrigger/versions/TrigNavigation_v1.h"
#include "xAODTrigger/versions/TrigNavigationAuxInfo_v1.h"

#include "xAODTrigger/versions/TrigConfKeys_v1.h"

#include "xAODTrigger/versions/TrigComposite_v1.h"
#include "xAODTrigger/versions/TrigCompositeContainer_v1.h"
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v1.h"
#include "xAODTrigger/versions/TrigCompositeAuxContainer_v2.h"

#include "xAODTrigger/versions/BunchConfKey_v1.h"
#include "xAODTrigger/versions/BunchConfContainer_v1.h"
#include "xAODTrigger/versions/BunchConfAuxContainer_v1.h"

#include "xAODTrigger/versions/TrigPassBits_v1.h"
#include "xAODTrigger/versions/TrigPassBitsContainer_v1.h"
#include "xAODTrigger/versions/TrigPassBitsAuxContainer_v1.h"

#include "xAODTrigger/versions/RoiDescriptorStore_v1.h"
#include "xAODTrigger/versions/RoiDescriptorStoreAuxInfo_v1.h"

namespace{
  struct GCCXML_DUMMY_INSTANTIATION_XAODTRIGGER {

    xAOD::MuonRoIContainer_v1 muon_c1;
    DataLink< xAOD::MuonRoIContainer_v1 > muon_l1;
    ElementLink< xAOD::MuonRoIContainer_v1 > muon_l2;
    ElementLinkVector< xAOD::MuonRoIContainer_v1 > muon_l3;
    std::vector< DataLink<xAOD::MuonRoIContainer_v1 > > muon_l4;
    std::vector< ElementLink<xAOD::MuonRoIContainer_v1 > > muon_l5;
    std::vector< ElementLinkVector< xAOD::MuonRoIContainer_v1 > > muon_l6;

    xAOD::JetRoIContainer_v1 jet_c1;
    DataLink< xAOD::JetRoIContainer_v1 > jet_l1;
    ElementLink< xAOD::JetRoIContainer_v1 > jet_l2;
    ElementLinkVector< xAOD::JetRoIContainer_v1 > jet_l3;
    std::vector< DataLink<xAOD::JetRoIContainer_v1 > > jet_l4;
    std::vector< ElementLink<xAOD::JetRoIContainer_v1 > > jet_l5;
    std::vector< ElementLinkVector< xAOD::JetRoIContainer_v1 > > jet_l6;

    xAOD::JetRoIContainer_v2 jet_v2_c1;
    DataLink< xAOD::JetRoIContainer_v2 > jet_v2_l1;
    ElementLink< xAOD::JetRoIContainer_v2 > jet_v2_l2;
    ElementLinkVector< xAOD::JetRoIContainer_v2 > jet_v2_l3;
    std::vector< DataLink<xAOD::JetRoIContainer_v2 > > jet_v2_l4;
    std::vector< ElementLink<xAOD::JetRoIContainer_v2 > > jet_v2_l5;
    std::vector< ElementLinkVector< xAOD::JetRoIContainer_v2 > > jet_v2_l6;

    xAOD::EmTauRoIContainer_v1 emtau_c1;
    DataLink< xAOD::EmTauRoIContainer_v1 > emtau_l1;
    ElementLink< xAOD::EmTauRoIContainer_v1 > emtau_l2;
    ElementLinkVector< xAOD::EmTauRoIContainer_v1 > emtau_l3;
    std::vector< DataLink<xAOD::EmTauRoIContainer_v1 > > emtau_l4;
    std::vector< ElementLink<xAOD::EmTauRoIContainer_v1 > > emtau_l5;
    std::vector< ElementLinkVector< xAOD::EmTauRoIContainer_v1 > > emtau_l6;

    xAOD::EmTauRoIContainer_v2 emtau_v2_c1;
    DataLink< xAOD::EmTauRoIContainer_v2 > emtau_v2_l1;
    ElementLink< xAOD::EmTauRoIContainer_v2 > emtau_v2_l2;
    ElementLinkVector< xAOD::EmTauRoIContainer_v2 > emtau_v2_l3;
    std::vector< DataLink<xAOD::EmTauRoIContainer_v2 > > emtau_v2_l4;
    std::vector< ElementLink<xAOD::EmTauRoIContainer_v2 > > emtau_v2_l5;
    std::vector< ElementLinkVector< xAOD::EmTauRoIContainer_v2 > > emtau_v2_l6;

    DataLink< xAOD::JetEtRoI_v1 > jetEt_l1;
    std::vector< DataLink< xAOD::JetEtRoI_v1 > > jetEt_l2;

    DataLink< xAOD::EnergySumRoI_v1 > esum_l1;
    std::vector< DataLink< xAOD::EnergySumRoI_v1 > > esum_l2;

    DataLink< xAOD::EnergySumRoI_v2 > esum_v2_l1;
    std::vector< DataLink< xAOD::EnergySumRoI_v2 > > esum_v2_l2;

    xAOD::TriggerMenuContainer_v1 c2;

    xAOD::TrigCompositeContainer_v1 comp_c1;

    xAOD::BunchConfContainer_v1 c5;

    xAOD::TrigPassBitsContainer_v1 pb_c1;
    DataLink< xAOD::TrigPassBitsContainer_v1 > pb_l1;
    ElementLink< xAOD::TrigPassBitsContainer_v1 > pb_l2;
    std::vector< DataLink< xAOD::TrigPassBitsContainer_v1 > > pb_l3;
    std::vector< ElementLink< xAOD::TrigPassBitsContainer_v1 > > pb_l4;

  };
}

#endif // XAODTRIGGER_XAODTRIGGERDICT_H
