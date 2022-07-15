/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef Jet_Manager_H
#define Jet_Manager_H

#include "AthenaBaseComps/AthAlgTool.h"
#include "src/TrigBtagEmulationJet.h"

#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/ReadHandleKey.h"

#include "xAODTrigger/JetRoIContainer.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODBTagging/BTaggingUtilities.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include "TLorentzVector.h"
#include <string>
#include <vector>

namespace Trig {

  class JetManagerTool : 
    public AthAlgTool {
  public:
    JetManagerTool(const std::string& type, 
		   const std::string& name, 
		   const IInterface* parent);
    virtual ~JetManagerTool() = default;
    
    virtual StatusCode initialize() override;
        
    StatusCode retrieveByNavigation();
    StatusCode retrieveByContainer();
    
    const std::string& chainName() const;
    const std::string& jetContainerName() const;    
    unsigned int jetSize() const;
    unsigned int jetRoISize() const;
    unsigned int btaggingSize() const;
    
    struct PreselJet {
      double pt;
      double eta;
      TLorentzVector p4;
    };
    
    const std::vector< std::shared_ptr< TrigBtagEmulationJet > >& getJets() const;
    const std::vector<PreselJet>& getSortedPreselJets() const;
    
  private:
    
    bool clear();

    template<typename external_collection_t> 
      bool getFromCombo(std::unique_ptr< external_collection_t >&, 
			const Trig::Combination&,
			const std::string& key = "");
    
    void jetCopy( const std::unique_ptr< xAOD::JetContainer >& );
    void jetCopy( const std::unique_ptr< xAOD::JetRoIContainer>& );
    
  private:
    PublicToolHandle< Trig::TrigDecisionTool > m_trigDec {this, "TrigDecisionTool", "","Trigger Decision Tool"};
    SG::ReadHandleKey< xAOD::JetContainer > m_jetInputKey {this, "InputJets", "Unspecified", "Input Jet Collection Key, retrieved from reconstructed jets"};
    SG::ReadHandleKey< xAOD::JetRoIContainer > m_jetRoIInputKey {this, "InputJetRoIs", "Unspecified", "Input Jet RoI Collection Key, retrieved from reconstructed jets"};

    // Chain: source of unbiased b-jets 
    Gaudi::Property<std::string> m_chain {this, "ChainName", "", "Chain: source of unbiased b-jets "};    
    // Container
    Gaudi::Property<std::string> m_jetcontainer {this, "JetContainerName", "", "Jet Container"};
    // Keys for Navigation
    Gaudi::Property<std::string> m_jetKey {this, "JetNavigationKey", ""};
    Gaudi::Property<std::string> m_btagKey {this, "BtagNavigationKey", ""};
    // For jetRoI, see if 4x4 should be used instead of 8x8
    Gaudi::Property<bool> m_uses4x4 {this, "Use4x4", false};
    
    // Local containers
    std::unique_ptr< xAOD::JetContainer > m_jet_Containers;
    std::unique_ptr< xAOD::JetAuxContainer > m_jet_Containers_Aux;

    std::unique_ptr< xAOD::JetRoIContainer > m_jetRoI_Containers;
    std::unique_ptr< xAOD::JetRoIAuxContainer > m_jetRoI_Containers_Aux;

    std::unique_ptr< xAOD::BTaggingContainer > m_btagging_Containers;
    std::unique_ptr< xAOD::BTaggingAuxContainer > m_btagging_Containers_Aux;
    
    std::vector< std::shared_ptr< TrigBtagEmulationJet > > m_outputJets;
    std::vector<PreselJet> m_sortedPreselJets;
    
  };

  inline const std::string& JetManagerTool::chainName() const { return m_chain.value(); }
  inline const std::string& JetManagerTool::jetContainerName() const { return m_jetcontainer.value(); }
  inline unsigned int JetManagerTool::jetSize() const { return m_jet_Containers->size(); }
  inline unsigned int JetManagerTool::jetRoISize() const { return m_jetRoI_Containers->size(); }
  inline unsigned int JetManagerTool::btaggingSize() const { return m_btagging_Containers->size(); }
  inline const std::vector< std::shared_ptr< TrigBtagEmulationJet > >& JetManagerTool::getJets() const { return m_outputJets; }
  inline const std::vector<JetManagerTool::PreselJet>& JetManagerTool::getSortedPreselJets() const { return m_sortedPreselJets; }
} // namespace

#endif
