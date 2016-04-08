/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigParticle/TrigElectronContainer.h"
#include "TrigParticle/TrigPhotonContainer.h"
#include "TrigParticle/TrigTauContainer.h"
#include "TrigParticle/TrigL2Bphys.h"
#include "TrigParticle/TrigL2BphysContainer.h"
#include "TrigParticle/TrigEFBphys.h"
#include "TrigParticle/TrigEFBphysContainer.h"
#include "TrigParticle/TrigL2BjetContainer.h"
#include "TrigParticle/TrigEFBjetContainer.h"
#include "JetEvent/JetCollection.h"
#include "JetEvent/JetKeyDescriptor.h"
#include "JetEvent/JetKeyDescriptorCollection.h"
#include "JetEvent/JetMomentMap.h"
#include "JetEvent/JetMomentMapCollection.h"
#include "egammaEvent/egammaContainer.h"
#include "egammaEvent/egDetailContainer.h"
#include "tauEvent/TauDetailsContainer.h"
#include "tauEvent/TauJetContainer.h"
#include "xAODJet/JetContainer.h" 
#include "xAODTrigEgamma/TrigElectronContainer.h"
#include "xAODTrigEgamma/TrigElectron.h"
#include "xAODTrigEgamma/TrigPhotonContainer.h"
#include "xAODTrigEgamma/TrigPhoton.h"

#include "TrigNavigation/TypeRegistration.h"
REGISTER_PACKAGE_WITH_NAVI(TrigParticle)

// Avoid checkreq warnings.
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODTau/TauJetContainer.h"
