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

  m_jetInputKey = m_jetcontainer.value();
  m_bjetInputKey = m_jetcontainer.value() + "_bJets";

  ATH_CHECK(m_jetInputKey.initialize());
  ATH_CHECK(m_bjetInputKey.initialize());

  return StatusCode::SUCCESS;
}
  
StatusCode JetManagerTool::retrieveByContainer(const EventContext& ctx,
					       EmulContext& emulCtx) const
{
  ATH_MSG_DEBUG( "Retrieving via Container ..." );

  auto outputJets = std::make_unique<std::vector<TrigBtagEmulationJet>>();
  auto sortedPreselJets = std::make_unique<std::vector<TrigBtagEmulationJet>>();

  // Get Jet Objects
  // We are retrieving xAOD::Jet object
  // This will first retrieve the jets in the event given the jet input key <jet-key>
  // then it retrieves the corresponding b-jet collection <jet-key>_bJets
  // replacing the non-b-jets with the b-jets

  // Retrieve jets
  ATH_MSG_DEBUG("Retrieving jet collection: " << m_jetInputKey.key());
  SG::ReadHandle< xAOD::JetContainer > jetContainerHandle = SG::makeHandle( m_jetInputKey, ctx );
  ATH_CHECK(jetContainerHandle.isValid());
  const xAOD::JetContainer* theJetContainer = jetContainerHandle.cptr();
  // Put in storage    
  outputJets->reserve(theJetContainer->size());
  sortedPreselJets->reserve(theJetContainer->size());
  for ( const xAOD::Jet *jet : *theJetContainer ) {
    TrigBtagEmulationJet toAdd(*jet, m_btagging_link.value());
    outputJets->push_back( toAdd ); 
  }

  // Retrieve b-Jets
  ATH_MSG_DEBUG("Retrieving b-jet collection: " << m_bjetInputKey.key());
  SG::ReadHandle< xAOD::JetContainer > bjetContainerHandle = SG::makeHandle( m_bjetInputKey, ctx );
  // if not valid, it means there is no corresponding b-jet collection
  // this happens for instance for presel jets
  // so this is ok
  if ( bjetContainerHandle.isValid() ) { 
    ATH_CHECK( bjetContainerHandle.isValid() );
    const xAOD::JetContainer* theBJetContainer = bjetContainerHandle.cptr();
  
    // Replace jets with b-jets
    // - loop on bjets
    for ( const xAOD::Jet *bjet : *theBJetContainer ) {
      // - loop on stored jets
      for (std::size_t ijet(0); ijet < outputJets->size(); ijet++) {
	const auto& emuljet = outputJets->at(ijet);
	const xAOD::Jet* jet = emuljet.jet();
	
	// To-Do: find better way
	if ( bjet->pt() == jet->pt() &&
	     bjet->eta() == jet->eta() &&
	     bjet->phi() == jet->phi() ) {
	  outputJets->at(ijet) = TrigBtagEmulationJet(*bjet, m_btagging_link.value());
	  break;
	}
      }
    }
  } // is valid
  
  // Prepare presel jets
  for ( const auto& jet : *outputJets.get() ) {
    const xAOD::Jet *theJet = jet.jet();
    sortedPreselJets->push_back( TrigBtagEmulationJet(*theJet) );
  }


  // Sort presel jets
  sort(sortedPreselJets->begin(), sortedPreselJets->end(), 
       [] (const auto& lhs, const auto& rhs) -> bool
       { return lhs.pt() > rhs.pt(); }
       );

  ATH_MSG_DEBUG( " - Ten largest jets:");
  for(unsigned int i = 0; i < 10 and i < sortedPreselJets->size(); i++) {
    ATH_MSG_DEBUG( " - pt=" << (sortedPreselJets->at(i).pt() / Gaudi::Units::GeV) << " eta=" << sortedPreselJets->at(i).eta() );
  }

  // Store objects
  const std::string storage_name = m_jetInputKey.key();
  emulCtx.store( storage_name, std::move(outputJets) );
  emulCtx.store( storage_name + "_presel", std::move(sortedPreselJets) );

  return StatusCode::SUCCESS;
}

const std::vector<TrigBtagEmulationJet>& JetManagerTool::getJets(const EmulContext& emulCtx) const 
{
  return *emulCtx.get<std::vector<TrigBtagEmulationJet>>(m_jetcontainer.value());
}
const std::vector<TrigBtagEmulationJet>& JetManagerTool::getSortedPreselJets(const EmulContext& emulCtx) const 
{ 
  return *emulCtx.get<std::vector<TrigBtagEmulationJet>>(m_jetcontainer.value() + "_presel");
}

}

//**********************************************************************
