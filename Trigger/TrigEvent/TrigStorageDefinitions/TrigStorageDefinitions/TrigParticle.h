/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGSTORAGEDEF_TRIGPARTICLEEVENT
#define TRIGSTORAGEDEF_TRIGPARTICLEEVENT
#include "TrigStorageDefinitions/EDM_TypeInformation.h"

namespace Analysis{
  class TauDetails;
  class TauDetailsContainer;
  class TauJet;
  class TauJetContainer;  
}

#include "xAODEgamma/ElectronContainer.h"
#include "xAODEgamma/ElectronAuxContainer.h"

#include "xAODEgamma/PhotonContainer.h"
#include "xAODEgamma/PhotonAuxContainer.h"

#include "xAODTau/TauJetContainer.h"
#include "xAODTau/TauJetAuxContainer.h"
#include "xAODTau/TauTrackContainer.h"
#include "xAODTau/TauTrackAuxContainer.h"

#include "xAODJet/JetContainer.h"
#include "xAODJet/JetTrigAuxContainer.h"

#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectronAuxContainer.h"


#include "xAODTrigEgamma/TrigPhoton.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhotonAuxContainer.h"
#include "xAODTrigEgamma/ElectronTrigAuxContainer.h"
#include "xAODTrigEgamma/PhotonTrigAuxContainer.h"


#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingTrigAuxContainer.h"

#include "xAODBTagging/BTagVertex.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"


HLT_BEGIN_TYPE_REGISTRATION
    // Level2
  HLT_REGISTER_TYPE(class TrigElectron, class TrigElectronContainer,class  TrigElectronContainer)
  HLT_REGISTER_TYPE(class TrigL2Bphys, class  TrigL2BphysContainer, class TrigL2BphysContainer)
  HLT_REGISTER_TYPE(class TrigPhoton, class TrigPhotonContainer,class TrigPhotonContainer)
  HLT_REGISTER_TYPE(class TrigTau, class TrigTau,class TrigTauContainer)
  HLT_REGISTER_TYPE(class TrigL2Bjet, class TrigL2BjetContainer,class TrigL2BjetContainer)
     // EF
  HLT_REGISTER_TYPE(class TrigEFBphys, class  TrigEFBphysContainer,class TrigEFBphysContainer)
  HLT_REGISTER_TYPE(class TrigEFBjet, class TrigEFBjetContainer,class TrigEFBjetContainer)
  HLT_REGISTER_TYPE(class JetKeyDescriptor, class JetKeyDescriptor,class JetKeyDescriptorCollection )
  HLT_REGISTER_TYPE(class JetMomentMap,class  JetMomentMap,class JetMomentMapCollection )
  HLT_REGISTER_TYPE(class Jet, class  JetCollection,class JetCollection )
  HLT_REGISTER_TYPE(class egamma, class  egammaContainer,class egammaContainer)
  HLT_REGISTER_TYPE(class egDetail, class egDetailContainer, class egDetailContainer )
  HLT_REGISTER_TYPE(Analysis::TauDetails, Analysis::TauDetailsContainer,  Analysis :: TauDetailsContainer )
  HLT_REGISTER_TYPE(Analysis::TauJet, Analysis::TauJetContainer, Analysis::TauJetContainer )

  // xAOD
  //  HLT_REGISTER_TYPE(xAOD::Egamma, xAOD::EgammaContainer, xAOD::EgammaContainer, xAOD::EgammaAuxContainer)  
  HLT_REGISTER_TYPE(xAOD::Electron, xAOD::ElectronContainer, xAOD::ElectronContainer, xAOD::ElectronTrigAuxContainer)
  HLT_REGISTER_TYPE(xAOD::Photon, xAOD::PhotonContainer, xAOD::PhotonContainer, xAOD::PhotonTrigAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TauJet, xAOD::TauJetContainer, xAOD::TauJetContainer, xAOD::TauJetAuxContainer)  
  HLT_REGISTER_TYPE(xAOD::TauTrack, xAOD::TauTrackContainer, xAOD::TauTrackContainer, xAOD::TauTrackAuxContainer)
  HLT_REGISTER_TYPE(xAOD::Jet, xAOD::JetContainer, xAOD::JetContainer, xAOD::JetTrigAuxContainer)

  HLT_REGISTER_TYPE(xAOD::TrigElectron, xAOD::TrigElectronContainer, xAOD::TrigElectronContainer, xAOD::TrigElectronAuxContainer)
  HLT_REGISTER_TYPE(xAOD::TrigPhoton, xAOD::TrigPhotonContainer, xAOD::TrigPhotonContainer, xAOD::TrigPhotonAuxContainer)
  HLT_REGISTER_TYPE(xAOD::BTagging, xAOD::BTaggingContainer, xAOD::BTaggingContainer, xAOD::BTaggingTrigAuxContainer)
  HLT_REGISTER_TYPE(xAOD::BTagVertex, xAOD::BTagVertexContainer, xAOD::BTagVertexContainer, xAOD::BTagVertexAuxContainer)
  
HLT_END_TYPE_REGISTRATION(TrigParticle)

#endif
