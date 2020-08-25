/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include <memory>
#include "AsgDataHandles/ReadHandle.h"
#include "JetRec/JetClusterer.h"
#include "fastjet/PseudoJet.hh"
#include "fastjet/ClusterSequence.hh"
#include "fastjet/ClusterSequenceArea.hh"
#include "fastjet/config.h"

#include "xAODEventInfo/EventInfo.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAuxContainer.h"

#include "JetEDM/FastJetUtils.h"

#include "JetRec/PseudoJetTranslator.h"


namespace JetClustererHelper {

  void seedsFromEventInfo(const xAOD::EventInfo* ei, std::vector<int> &seeds){
      //const xAOD::EventInfo* pevinfo = handle.cptr();
      auto ievt = ei->eventNumber();
      auto irun = ei->runNumber();

      if ( ei->eventType(xAOD::EventInfo::IS_SIMULATION)) {
	// For MC, use the channel and MC event number
	ievt = ei->mcEventNumber();
	irun = ei->mcChannelNumber();
      }
      seeds.push_back(ievt);
      seeds.push_back(irun);
  }
}

StatusCode JetClusterer::initialize() {

  ATH_MSG_DEBUG("Initializing...");
  xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_jetalg);
  m_fjalg = xAOD::JetAlgorithmType::fastJetDef(ialg);
  if ( m_fjalg == fastjet::undefined_jet_algorithm ) {
    ATH_MSG_ERROR("Invalid jet algorithm name: " << m_jetalg);
    ATH_MSG_ERROR("Allowed values are Kt, CamKt, AntiKt, etc.");
    return StatusCode::FAILURE;
  }
  if ( m_jetrad <=0 ) {
    ATH_MSG_ERROR("Invalid jet size parameter: " << m_jetrad);
    return StatusCode::FAILURE;
  }

  // buld an empty ClusterSequence, just for the fastjet splash screen to appear during initialization (?)
  fastjet::JetDefinition jetdef(m_fjalg, m_jetrad);
  PseudoJetVector empty;
  fastjet::ClusterSequence cs(empty, jetdef);
  cs.inclusive_jets(m_ptmin);

  // Input DataHandles
  if( !m_finalPseudoJets.empty() ) {
    ATH_MSG_WARNING("A non-empty value was found for the FinalPseudoJets WriteHandleKey -- this will be ignored!");
  }

  ATH_CHECK( m_eventinfokey.initialize() );
  ATH_CHECK( m_inputPseudoJets.initialize() );
  m_finalPseudoJets = name() + "FinalPJ";
  ATH_CHECK( m_finalPseudoJets.initialize() );

  return StatusCode::SUCCESS;
}


std::pair<std::unique_ptr<xAOD::JetContainer>, std::unique_ptr<SG::IAuxStore> > JetClusterer::getJets() const {
  // Return this in case of any problems
  auto nullreturn = std::make_pair(std::unique_ptr<xAOD::JetContainer>(nullptr), std::unique_ptr<SG::IAuxStore>(nullptr));

  // -----------------------
  // retrieve input
  SG::ReadHandle<PseudoJetContainer> pjContHandle(m_inputPseudoJets);
  if(!pjContHandle.isValid()) {
    ATH_MSG_ERROR("No valid PseudoJetContainer with key "<< m_inputPseudoJets.key());
    return nullreturn;
  }

  // Build the container to be returned
  // Avoid memory leaks with unique_ptr
  auto jets = std::make_unique<xAOD::JetContainer>();
  auto auxCont = std::make_unique<xAOD::JetAuxContainer>();
  jets->setStore(auxCont.get());

  const PseudoJetVector* pseudoJetVector = pjContHandle->casVectorPseudoJet();
  ATH_MSG_DEBUG("Pseudojet input container has size " << pseudoJetVector->size());

  // -----------------------
  // Build the cluster sequence
  fastjet::JetDefinition jetdef(m_fjalg, m_jetrad);
  fastjet::ClusterSequence *clSequence = nullptr;
  bool useArea = m_ghostarea > 0 ;
  if ( useArea ) {
    // Prepare ghost area specifications -------------
    ATH_MSG_DEBUG("Creating input area cluster sequence");
    bool seedsok=true;
    fastjet::AreaDefinition adef = buildAreaDefinition(seedsok);
    if(seedsok) {clSequence = new fastjet::ClusterSequenceArea(*pseudoJetVector, jetdef, adef);}
    else {return nullreturn;}
  } else {
    ATH_MSG_DEBUG("Creating input cluster sequence");
    clSequence = new fastjet::ClusterSequence(*pseudoJetVector, jetdef);
  } 


  // -----------------------
  // Build a new pointer to a PseudoJetVector containing the final PseudoJet
  // This allows us to own the vector of PseudoJet which we will put in the evt store.
  // Thus the contained PseudoJet will be kept frozen there and we can safely use pointer to them from the xAOD::Jet objects
  auto pjVector = std::make_unique<PseudoJetVector>(fastjet::sorted_by_pt(clSequence->inclusive_jets(m_ptmin)) );
  ATH_MSG_DEBUG("Found jet count: " << pjVector->size());
  if(msgLvl(MSG::VERBOSE)) {
    for(const auto& pj : *pjVector) {
      msg() << "  Pseudojet with pt " << std::setprecision(4) << pj.Et()*1e-3 << " has " << pj.constituents().size() << " constituents" << endmsg;
    }
  }

  // Let fastjet deal with deletion of ClusterSequence, so we don't need to also put it in the EventStore.
  clSequence->delete_self_when_unused();


  // -------------------------------------
  // translate to xAOD::Jet
  ATH_MSG_DEBUG("Converting pseudojets to xAOD::Jet");
  static SG::AuxElement::Accessor<const fastjet::PseudoJet*> pjAccessor("PseudoJet");
  PseudoJetTranslator pjTranslator(useArea, useArea);
  for (const fastjet::PseudoJet &  pj: *pjVector ) {
    // create the xAOD::Jet from the PseudoJet, doing the signal & ghost constituents extraction
    xAOD::Jet& jet = pjTranslator.translate(pj, *pjContHandle, *jets);

    // Add the PseudoJet onto the xAOD jet. Maybe we should do it in the above JetFromPseudojet call ??
    pjAccessor(jet) = &pj;

    jet.setInputType(  xAOD::JetInput::Type( (int) m_inputType) ) ;
    xAOD::JetAlgorithmType::ID ialg = xAOD::JetAlgorithmType::algId(m_fjalg);
    jet.setAlgorithmType(ialg);
    jet.setSizeParameter((float)m_jetrad);
    if(useArea) jet.setAttribute(xAOD::JetAttribute::JetGhostArea, (float)m_ghostarea);

    ATH_MSG_VERBOSE( "  xAOD::Jet with pt " << std::setprecision(4) << jet.pt()*1e-3 << " has " << jet.getConstituents().size() << " constituents" );
    ATH_MSG_VERBOSE( "  Leading constituent is of type " << jet.getConstituents()[0].rawConstituent()->type());
  }

  // -------------------------------------
  // record final PseudoJetVector
  SG::WriteHandle<PseudoJetVector> pjVectorHandle(m_finalPseudoJets);
  if(!pjVectorHandle.record(std::move(pjVector))){
    ATH_MSG_ERROR("Can't record PseudoJetVector under key "<< m_finalPseudoJets);
    return nullreturn;
  }

  ATH_MSG_DEBUG("Reconstructed jet count: " << jets->size() <<  "  clusterseq="<<clSequence);
  // Return the jet container and aux, use move to transfer
  // ownership of pointers to caller
  return std::make_pair(std::move(jets), std::move(auxCont));
}



fastjet::AreaDefinition JetClusterer::buildAreaDefinition(bool & seedsok) const {

    fastjet::GhostedAreaSpec gspec(5.0, 1, m_ghostarea);
    seedsok = true;

    if ( m_ranopt == 1 ) {
      // Use run/event number as random number seeds.
      auto evtInfoHandle = SG::makeHandle(m_eventinfokey);
      if (!evtInfoHandle.isValid()){
        ATH_MSG_ERROR("Unable to retrieve event info");
	seedsok = false;
        return fastjet::AreaDefinition();
      }

      std::vector<int> inseeds;
      JetClustererHelper::seedsFromEventInfo(evtInfoHandle.cptr(), inseeds);
      gspec.set_random_status(inseeds);
    }


    ATH_MSG_DEBUG("Active area specs:");
    ATH_MSG_DEBUG("  Requested ghost area: " << m_ghostarea);
    ATH_MSG_DEBUG("     Actual ghost area: " << gspec.actual_ghost_area());
    ATH_MSG_DEBUG("               Max eta: " << gspec.ghost_etamax());
    ATH_MSG_DEBUG("              # ghosts: " << gspec.n_ghosts());
    ATH_MSG_DEBUG("       # rapidity bins: " << gspec.nrap());
    ATH_MSG_DEBUG("            # phi bins: " << gspec.nphi());
    std::vector<int> seeds;
    gspec.get_random_status(seeds);
    if ( seeds.size() == 2 ) {
      ATH_MSG_DEBUG("          Random seeds: " << seeds[0] << ", " << seeds[1]);
    } else {
      ATH_MSG_WARNING("Random generator size is not 2: " << seeds.size());
      ATH_MSG_DEBUG("          Random seeds: ");
      for ( auto seed : seeds ) ATH_MSG_DEBUG("                 " << seed);
    }

    return fastjet::AreaDefinition(fastjet::active_area, gspec);
}
