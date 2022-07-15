/*
Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration 
*/

#include "src/JetManagerTool.h"

namespace Trig {

//**********************************************************************

JetManagerTool::JetManagerTool(const std::string& type,	
			       const std::string& name, 
			       const IInterface* parent) 
  : AthAlgTool(type, name, parent)
{}
  
StatusCode JetManagerTool::initialize() {
  ATH_MSG_DEBUG( "Initializing " << name() );
  ATH_CHECK( m_trigDec.retrieve() );
  
  m_jet_Containers = std::make_unique<xAOD::JetContainer>();
  m_jet_Containers_Aux = std::make_unique<xAOD::JetAuxContainer>();
  m_jet_Containers->setStore( m_jet_Containers_Aux.get() );
  ATH_CHECK(m_jetInputKey.initialize());

  m_jetRoI_Containers = std::make_unique<xAOD::JetRoIContainer>();
  m_jetRoI_Containers_Aux = std::make_unique<xAOD::JetRoIAuxContainer>();
  m_jetRoI_Containers->setStore( m_jetRoI_Containers_Aux.get() );
  ATH_CHECK(m_jetRoIInputKey.initialize());
  
  m_btagging_Containers = std::make_unique<xAOD::BTaggingContainer>();
  m_btagging_Containers_Aux = std::make_unique<xAOD::BTaggingAuxContainer>();
  m_btagging_Containers->setStore( m_btagging_Containers_Aux.get() );
  
  return StatusCode::SUCCESS;
}

StatusCode JetManagerTool::retrieveByNavigation() {
  clear();
  ATH_MSG_DEBUG( "Retrieving via Navigation ..." );
  
  if (m_trigDec->getNavigationFormat() == "TriggerElement") {
    ATH_MSG_DEBUG( "Using Run 2 Navigation API." );
    
    Trig::FeatureContainer features = m_trigDec->features(m_chain);
    const std::vector< Trig::Combination >& combinations = features.getCombinations();
    
    for ( Trig::Combination combo : combinations ) {
      getFromCombo( m_jet_Containers, combo, m_jetKey);
    }
  }
  else {
    ATH_MSG_DEBUG( "Using Run 3 Navigation API." );
    
    const std::vector< TrigCompositeUtils::LinkInfo<xAOD::JetContainer> > fc_jet =
      m_trigDec->features<xAOD::JetContainer>( m_chain );
    
    for(const auto& jetLinkInfo : fc_jet) {
      if(jetLinkInfo.isValid()) {
	const xAOD::Jet *jet = *(jetLinkInfo.link);
	
	bool unique = true;
	for ( xAOD::Jet *storedJet : *m_jet_Containers.get() ) {
	  if ( jet->pt() == storedJet->pt() &&
	       jet->eta() == storedJet->eta() &&
	       jet->phi() == storedJet->phi() ) {
	    // if the duplicate has a btagging object linked to it, use that one instead
	    if(xAOD::BTaggingUtilities::getBTagging(*jet)) {
	      *storedJet = *jet;
	    }
	    
	    unique = false;
	    break;
	  }
	}
	
	if ( unique ) {
	  m_jet_Containers->push_back( new xAOD::Jet( *jet ) ); // push back a copy of the jet
	}
      }
    }
  }
  
  jetCopy( m_jet_Containers );
 
  return StatusCode::SUCCESS;
}

template<typename external_collection_t> 
bool JetManagerTool::getFromCombo(std::unique_ptr< external_collection_t > &output,
				  const Trig::Combination& combo,
				  const std::string& key) {
  using collection_t = typename std::remove_const< typename std::remove_pointer< typename external_collection_t::value_type >::type >::type;
  const std::vector< Trig::Feature< external_collection_t > > tmpFeatures = combo.containerFeature< external_collection_t >( key.c_str() );
  
  for ( const Trig::Feature< external_collection_t >& feature : tmpFeatures ) {
    const external_collection_t *tmpContainer = nullptr;
    tmpContainer = feature.cptr();
    
    if (tmpContainer == nullptr) {
      return false;
    }
    
    collection_t* toBeAdded = new collection_t();
    output->push_back( toBeAdded );
    *toBeAdded = *tmpContainer->at(0);
  }
  
  return true;
}
  
void JetManagerTool::jetCopy( const std::unique_ptr< xAOD::JetContainer >& container ) 
{
  for ( const xAOD::Jet *theJet : *container.get() ) {
    auto toAdd = std::make_shared< TrigBtagEmulationJet >(name() + ".TrigBtagEmulationJet", theJet);
    toAdd->setLevel(msgLevel());
    m_outputJets.push_back( toAdd );
    m_sortedPreselJets.push_back({theJet->pt(), theJet->eta(), theJet->p4()});
  }
  
  sort(m_sortedPreselJets.begin(), m_sortedPreselJets.end(), [](const auto& lhs, const auto& rhs){return (lhs.pt > rhs.pt);});
  ATH_MSG_DEBUG( " - Ten largest jets:");
  for(unsigned int i = 0; i < 10 && i < m_sortedPreselJets.size(); i++) {
    ATH_MSG_DEBUG( " - pt=" << (m_sortedPreselJets[i].pt / Gaudi::Units::GeV) << " eta=" << m_sortedPreselJets[i].eta );
  }
}

void JetManagerTool::jetCopy( const std::unique_ptr< xAOD::JetRoIContainer >& container ) {
  for ( const xAOD::JetRoI *theJetRoI : *container.get() ) {
    auto toAdd = std::make_shared< TrigBtagEmulationJet >( name() + ".TrigBtagEmulationJet", theJetRoI, m_uses4x4 );
    toAdd->setLevel(msgLevel());
    m_outputJets.push_back( toAdd );
    
  }
}

StatusCode JetManagerTool::retrieveByContainer() {
  clear();
  ATH_MSG_DEBUG( "Retrieving via Container ..." );
  
  // In case we want LVL1 Jet we shoud use jet RoIs
  const xAOD::JetRoIContainer *theJetRoIContainer = nullptr;
  const xAOD::JetContainer *theJetContainer = nullptr;
  const std::string ilContainer = m_jetcontainer;
  
  // Get Jet Objects
  if ( ilContainer == "LVL1JetRoIs" ) {
    // We are retrieving xAOD::JetRoI object
    m_jetRoIInputKey = ilContainer;
    SG::ReadHandle< xAOD::JetRoIContainer > jetRoIContainerHandle = SG::makeHandle( m_jetRoIInputKey, Gaudi::Hive::currentContext() );
    if(!jetRoIContainerHandle.isValid()) {
      ATH_MSG_ERROR( "CANNOT RETRIEVE JET-ROI CONTAINER '" << m_jetRoIInputKey << "'" );
      return StatusCode::FAILURE;
    }
    theJetRoIContainer = jetRoIContainerHandle.get();
    
    for ( const xAOD::JetRoI *jetRoI : *theJetRoIContainer ) {
      bool unique = true;
      for ( const xAOD::JetRoI *storedJetRoI : *m_jetRoI_Containers.get() ) {
	if ( jetRoI->et4x4() == storedJetRoI->et4x4() &&
	     jetRoI->et8x8() == storedJetRoI->et8x8() &&
	     jetRoI->eta() == storedJetRoI->eta() &&
	     jetRoI->phi() == storedJetRoI->phi() ) {
	  unique = false;
	}
      }
      
      if (unique) { 
	xAOD::JetRoI *toBeAdded = new xAOD::JetRoI();
	m_jetRoI_Containers->push_back( toBeAdded );
	*toBeAdded = *jetRoI;
      }
    }
    
    jetCopy( m_jetRoI_Containers );
  }
  else {
    // We are retrieving xAOD::Jet object
    m_jetInputKey = ilContainer;
    SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_jetInputKey, Gaudi::Hive::currentContext() );
    if(!jetContainerHandle.isValid()) {
      ATH_MSG_ERROR( "Cannot retrieve jet container " << m_jetInputKey );
      return StatusCode::FAILURE;
    }
    theJetContainer = jetContainerHandle.get();
    
    for ( const xAOD::Jet *jet : *theJetContainer ) {
      bool unique = true;
      for ( const xAOD::Jet *storedJet : *m_jet_Containers.get() ) {
	if ( jet->pt() == storedJet->pt() &&
	     jet->eta() == storedJet->eta() &&
	     jet->phi() == storedJet->phi() ) {
	  unique = false;
	  break;
	}
      }
      
      if ( unique ) {
	xAOD::Jet *toBeAdded = new xAOD::Jet();
	m_jet_Containers->push_back( toBeAdded );
	*toBeAdded = *jet;
      }
    }
    
    // and now the bjets, which will all overlap with the jets we already have
    m_jetInputKey = ilContainer + "_bJets";
    jetContainerHandle = SG::makeHandle( m_jetInputKey, Gaudi::Hive::currentContext() );
    if(jetContainerHandle.isValid()) {
      theJetContainer = jetContainerHandle.get();
      
      for ( const xAOD::Jet *jet : *theJetContainer ) {
	for ( xAOD::Jet *storedJet : *m_jet_Containers.get() ) {
	  if ( jet->pt() == storedJet->pt() &&
	       jet->eta() == storedJet->eta() &&
	       jet->phi() == storedJet->phi() ) {
	    *storedJet = *jet; // deep copy
	    break;
	  }
	}
      }
    }
    else {
      ATH_MSG_INFO( "Couldn't retrieve b-jet container " << m_jetInputKey << ", maybe no b-jet in the event? If there are, this is an ERROR");
    }
    
    jetCopy( m_jet_Containers );
  }
  
  return StatusCode::SUCCESS;
}

bool JetManagerTool::clear() {
  m_jet_Containers->clear();
  m_jetRoI_Containers->clear();
  m_btagging_Containers->clear();
  m_outputJets.clear();
  m_sortedPreselJets.clear(); 
  return true;
}

}

//**********************************************************************
