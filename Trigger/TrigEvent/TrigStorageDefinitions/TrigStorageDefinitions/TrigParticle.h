/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGPARTICLEEVENT
#define TRIGSTORAGEDEF_TRIGPARTICLEEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

namespace Analysis{
  struct TauDetails;
  struct TauDetailsContainer;
  struct TauJet;
  struct TauJetContainer;  
}

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"

#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"


#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"



#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"


HLT_BEGIN_TYPE_REGISTRATION
    // Level2
  HLT_REGISTER_TYPE(struct TrigElectron, struct TrigElectronContainer,struct  TrigElectronContainer)
  HLT_REGISTER_TYPE(struct TrigL2Bphys, struct  TrigL2BphysContainer, struct TrigL2BphysContainer)
  HLT_REGISTER_TYPE(struct TrigPhoton, struct TrigPhotonContainer,struct TrigPhotonContainer)
  HLT_REGISTER_TYPE(struct TrigTau, struct TrigTau,struct TrigTauContainer)
  HLT_REGISTER_TYPE(struct TrigL2Bjet, struct TrigL2BjetContainer,struct TrigL2BjetContainer)
     // EF
  HLT_REGISTER_TYPE(struct TrigEFBphys, struct  TrigEFBphysContainer,struct TrigEFBphysContainer)
  HLT_REGISTER_TYPE(struct TrigEFBjet, struct TrigEFBjetContainer,struct TrigEFBjetContainer)
  HLT_REGISTER_TYPE(struct JetKeyDescriptor, struct JetKeyDescriptor,struct JetKeyDescriptorCollection )
  HLT_REGISTER_TYPE(struct JetMomentMap,struct  JetMomentMap,struct JetMomentMapCollection )
  HLT_REGISTER_TYPE(struct Jet, struct  JetCollection,struct JetCollection )
  HLT_REGISTER_TYPE(struct egamma, struct  egammaContainer,struct egammaContainer)
  HLT_REGISTER_TYPE(struct egDetail, struct egDetailContainer, struct egDetailContainer )
  HLT_REGISTER_TYPE(Analysis::TauDetails, Analysis::TauDetailsContainer,  Analysis :: TauDetailsContainer )
  HLT_REGISTER_TYPE(Analysis::TauJet, Analysis::TauJetContainer, Analysis::TauJetContainer )

  // xAOD
  //  HLT_REGISTER_TYPE(xAOD::Egamma, xAOD::EgammaContainer, xAOD::EgammaContainer, xAOD::EgammaAuxContainer)  
  HLT_REGISTER_TYPE(xAOD::Electron, xAOD::ElectronContainer, xAOD::ElectronContainer, xAOD::ElectronAuxContainer)  
  HLT_REGISTER_TYPE(xAOD::Photon, xAOD::PhotonContainer, xAOD::PhotonContainer, xAOD::PhotonAuxContainer)  
  HLT_REGISTER_TYPE(xAOD::TauJet, xAOD::TauJetContainer, xAOD::TauJetContainer, xAOD::TauJetAuxContainer)  
  HLT_REGISTER_TYPE(xAOD::Jet, xAOD::JetContainer, xAOD::JetContainer, xAOD::JetTrigAuxContainer)

  HLT_REGISTER_TYPE(xAOD::TrigElectron, xAOD::TrigElectronContainer, xAOD::TrigElectronContainer, xAOD::TrigElectronAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigPhoton, xAOD::TrigPhotonContainer, xAOD::TrigPhotonContainer, xAOD::TrigPhotonAuxContainer)
  HLT_REGISTER_TYPE(xAOD::BTagging, xAOD::BTaggingContainer, xAOD::BTaggingContainer, xAOD::BTaggingAuxContainer)
  
HLT_END_TYPE_REGISTRATION(TrigParticle)

#endif
