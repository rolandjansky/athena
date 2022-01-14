/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigBtagTLATool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************


#include "TrigBtagTLATool.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODBTagging/BTaggingUtilities.h"
#include "xAODBase/IParticleHelpers.h" //for getOriginalObjectLink

#include "GaudiKernel/StatusCode.h"

using TrigCompositeUtils::LinkInfo;

TrigBtagTLATool::TrigBtagTLATool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent){

}

TrigBtagTLATool::~TrigBtagTLATool(){
}

StatusCode TrigBtagTLATool::initialize(){
  ATH_MSG_INFO("Initializing " << name() << "...");
  CHECK( m_outputBTagInfoKey.initialize() );
  m_bTagJetDecorLinkName = m_outputBTagInfoKey.key()+".jetLink";
  CHECK( m_bTagJetDecorLinkName.initialize() );
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagTLATool::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode TrigBtagTLATool::record_btag(const xAOD::TrigCompositeContainer* previousDecisions, const xAOD::JetContainer& jets_to_btag, const EventContext& ctx)const{


  //###########################################################
  // Making a new output HLT BTagging container
  //###########################################################
  SG::WriteHandle<xAOD::BTaggingContainer> h_outBTagInfo = SG::makeHandle(m_outputBTagInfoKey, ctx );
  ATH_MSG_DEBUG("Recording BTagging info as " << h_outBTagInfo.key());
  ATH_CHECK( h_outBTagInfo.record (std::make_unique<xAOD::BTaggingContainer>(),
        std::make_unique<xAOD::BTaggingAuxContainer>() ) );
  SG::WriteDecorHandle<xAOD::BTaggingContainer, ElementLink< xAOD::JetContainer > > h_bTagJetLinkName(m_bTagJetDecorLinkName, ctx);

  //###########################################################
  // Now recording available BTagging information for each jet with steps:
  // (1) For each previous trigger decision, search for attached BTagging element
  // (2) If found a BTagging element, retrieve the linked b-jet and its original jet link ("getOriginalObjectLink")
  // (3) Find the matching jet in jets_to_btag via an identical original jet link. 
  // (4) Duplcate the BTagging element and attach to it a jet element link pointing to the matched jet
  // (5) Record the duplicated BTagging element in the newly created HLT BTagging container.
  //###########################################################

  for (const auto previousDecision : *previousDecisions)
  {
     std::vector<LinkInfo<xAOD::BTaggingContainer>> btagDecisionInfoLinks = TrigCompositeUtils::findLinks<xAOD::BTaggingContainer>(previousDecision, "btag", TrigDefs::lastFeatureOfType);
     if (btagDecisionInfoLinks.size() == 0 ){
         ATH_MSG_DEBUG("Skipping decision with no b-tag information.");
         continue;
     }
     if (btagDecisionInfoLinks.size() > 1 ){
       ATH_MSG_ERROR("More than one BTagging element found for this decision!");
       return StatusCode::FAILURE;
     }

     for (auto btagLinkInfo: btagDecisionInfoLinks){
       ElementLink<xAOD::BTaggingContainer> btagDecisionLink = btagLinkInfo.link;
       ATH_CHECK(btagDecisionLink.isValid());
       const xAOD::BTagging * btag = *btagDecisionLink;
       static const SG::AuxElement::ConstAccessor<ElementLink<xAOD::JetContainer>> getBJet("jetLink"); 
       const xAOD::Jet* bjet(nullptr);
       if (getBJet.isAvailable(*btag) && getBJet(*btag).isValid()) {
         bjet = *(getBJet(*btag));
       } 
       else return StatusCode::FAILURE;

       auto blink = getOriginalObjectLink(*(bjet));
       bool matched(false);
       for (auto jet: jets_to_btag ){
         auto jlink  = getOriginalObjectLink(*(jet));
         if (jlink == blink) {
           matched=true;
           if (jet->eta() != bjet->eta()) ATH_MSG_WARNING("Found matching jet for b-jet but their eta values differ (jet eta "<<jet->eta()<<", b-jet eta "<<bjet->eta()<<". Were these jets properly matched?");
           const xAOD::BTagging* bTagInfo = xAOD::BTaggingUtilities::getBTagging( *bjet );
           xAOD::BTagging * copied_bTagInfo = new xAOD::BTagging();
           h_outBTagInfo->push_back(copied_bTagInfo);
           *copied_bTagInfo = *bTagInfo;
           ElementLink<xAOD::JetContainer> jetEL;
           jetEL.toContainedElement(jets_to_btag ,jet);
           h_bTagJetLinkName(*h_outBTagInfo->back()) = jetEL;
           break;
         }
       } 
       if (!matched) ATH_MSG_WARNING("No matching jet found?");
     }        
  } 

  return StatusCode::SUCCESS;
}


