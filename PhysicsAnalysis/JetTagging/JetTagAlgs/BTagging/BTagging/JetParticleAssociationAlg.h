/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef BTAGGING_JETPARTICLEASSOCIATIONALG_HH
#define BTAGGING_JETPARTICLEASSOCIATIONALG_HH
///////////////////////////////////////////
///
/// \class JetParticleAssociationAlg
/// Algorithm to run dual tool JetParticleAssociation.
////////////////////////////////////////////

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"
#include "StoreGate/WriteHandleKey.h"
#include "xAODJet/JetContainer.h"
#include "xAODTracking/TrackParticle.h"
#include "ParticleJetTools/JetParticleAssociation.h"

namespace Analysis{
  class ParticleToJetAssociator;

  class  JetParticleAssociationAlg: public AthAlgorithm
    { 
      public:
  
        /** Constructors and destructors */
        JetParticleAssociationAlg(const std::string& name, ISvcLocator *pSvcLocator);
        virtual ~JetParticleAssociationAlg();
    
        /** Main routines specific to an ATHENA algorithm */
        virtual StatusCode initialize();
        virtual StatusCode execute();

      private:
  
        SG::ReadHandleKey<xAOD::JetContainer > m_JetCollectionName {this, "JetCollectionName", "", "Input jet container"};
        SG::ReadHandleKey<xAOD::TrackParticleContainer> m_TrackParticleContainerName{ this, "TrackParticleCollectionName", "", "Track particle container"};
        Gaudi::Property<SG::WriteDecorHandleKey<xAOD::JetContainer>> m_jetParticleLinkName{ this, "TrackToJetAssociatorName", "", "Element Link vector form jet to particle container"};

        ToolHandle< JetParticleAssociation> m_assoc;
        ToolHandle< Analysis::ParticleToJetAssociator > m_TrackToJetAssociator;

        std::string m_release;
   };

}

#endif
