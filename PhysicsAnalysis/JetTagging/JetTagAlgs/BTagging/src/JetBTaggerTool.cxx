/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/JetBTaggerTool.h"

#include "xAODCore/ShallowCopy.h"
#include "StoreGate/WriteDecorHandle.h"
#include "xAODJet/Jet.h"
#include "xAODJet/JetContainer.h"
#include "xAODBTagging/BTagging.h"
#include "xAODBTagging/BTaggingContainer.h"
#include "xAODBTagging/BTaggingAuxContainer.h"
#include "xAODTracking/VertexContainer.h"
#include "xAODTracking/VertexAuxContainer.h"
#include "xAODBTagging/BTagVertexContainer.h"
#include "xAODBTagging/BTagVertexAuxContainer.h"

#include "BTagging/BTagTool.h"
#include "BTagging/BTagTrackAssociation.h"
#include "BTagging/BTagSecVertexing.h"
#include "BTagging/BTagJetPtScaling.h"
#include "TRandom.h"

#include <iostream>
#include <string>

namespace Analysis {


JetBTaggerTool::JetBTaggerTool(const std::string& n) : 
  asg::AsgTool(n),
  m_BTagTrackAssocTool("Analysis::BTagTrackAssociation"),
  m_bTagSecVtxTool("Analysis::BTagSecVertexing"),
  m_PtRescalingTool("Analysis::BTagJetPtScaling"),
  m_augment(false),
  m_PtRescale(false),
  m_magFieldSvc("AtlasFieldSvc",n)
{

  declareProperty( "BTagTool", m_bTagTool);
  declareProperty( "BTagTrackAssocTool", m_BTagTrackAssocTool);
  declareProperty( "BTagSecVertexing", m_bTagSecVtxTool);
  declareProperty( "BTagAugmentation", m_augment, "switch to decide whether to merely extend the BTagging information as opposed to re-tagging from scratch");
  declareProperty( "BTagJetPtRescale", m_PtRescale, "switch to decide whether to carry out jet pt rescaling (to use calorimeter jet tunings for track jets)");
  declareProperty("MagFieldSvc",    m_magFieldSvc );
}

JetBTaggerTool::~JetBTaggerTool()
{
  /////....
}


StatusCode JetBTaggerTool::initialize() {
  // This will check that the properties were initialized properly
  // by job configuration.
  ATH_CHECK( m_JetCollectionName.initialize() );
  ATH_CHECK( m_BTaggingCollectionName.initialize() );
  m_jetBTaggingLinkName = m_JetCollectionName.key()+".btaggingLink";
  ATH_CHECK( m_jetBTaggingLinkName.initialize() );

  /// retrieve the track association tool
  if ( !m_BTagTrackAssocTool.empty() ) {
    if ( m_BTagTrackAssocTool.retrieve().isFailure() ) {
      ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_BTagTrackAssocTool);
      return StatusCode::FAILURE;
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_BTagTrackAssocTool);
    }
  }
  else {
     ATH_MSG_DEBUG("#BTAG# No track association tool to retrieve");
  }

  /// retrieve the main BTagTool
  if ( m_bTagTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_bTagTool);
    return StatusCode::FAILURE;
  } else {
    ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_bTagTool);
  }

  /// retrieve the bTagSecVtxTool
  if ( m_bTagSecVtxTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("#BTAGVTX# Failed to retrieve tool " << m_bTagSecVtxTool);
    return StatusCode::FAILURE;
  } else {
      ATH_MSG_DEBUG("#BTAGVTX# Retrieved tool " << m_bTagSecVtxTool); 
  }

  /// retrieve the jet pt rescaling tool
  if (m_PtRescalingTool.retrieve().isFailure()) {
    ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_PtRescalingTool);
    return StatusCode::FAILURE;
  } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_PtRescalingTool); 
  }

  /// retrieve the magnetic field service
  if (m_magFieldSvc.retrieve().isFailure()){
     ATH_MSG_ERROR("Could not get " << m_magFieldSvc); 
     return StatusCode::FAILURE;
  }

  return StatusCode::SUCCESS;
}


StatusCode JetBTaggerTool::execute() {
  //retrieve the Jet container
  SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName);
  if (!h_JetCollectionName.isValid()) {
    ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
    return StatusCode::FAILURE;
  }

  if (h_JetCollectionName->size() == 0) {
    ATH_MSG_DEBUG("#BTAG# Empty JetContainer !!");
  }
  else {
    ATH_MSG_DEBUG("#BTAG#  Nb jets in JetContainer: "<< h_JetCollectionName->size());
  }

  //Decor Jet with element link to the BTagging
  SG::WriteDecorHandle<xAOD::JetContainer,ElementLink< xAOD::BTaggingContainer > > h_jetBTaggingLinkName(m_jetBTaggingLinkName);

  //Create a xAOD::BTaggingContainer in any case (must be done) 
  std::string bTaggingContName = m_BTaggingCollectionName.key();
  ATH_MSG_DEBUG("#BTAG#  Container name: "<< bTaggingContName);

  /* Record the BTagging  output container */
  SG::WriteHandle<xAOD::BTaggingContainer> h_BTaggingCollectionName (m_BTaggingCollectionName);
  ATH_CHECK( h_BTaggingCollectionName.record(std::make_unique<xAOD::BTaggingContainer>(),
  					std::make_unique<xAOD::BTaggingAuxContainer>()) ); 

  if (!m_magFieldSvc->solenoidOn()) {
    for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
      const xAOD::Jet * jet = h_JetCollectionName->at(jetIndex); 
      ElementLink< xAOD::BTaggingContainer> linkBTagger;
      h_jetBTaggingLinkName(*jet) = linkBTagger;
    }
    return StatusCode::SUCCESS;
  }
  else { //Solenoid ON
    for (unsigned int i = 0; i < h_JetCollectionName->size(); i++) {
      xAOD::BTagging * newBTagMT  = new xAOD::BTagging();
      h_BTaggingCollectionName->push_back(newBTagMT);
    }
  }

     
  StatusCode jetIsAssociated;
  if (!m_BTagTrackAssocTool.empty()) {
    ATH_MSG_VERBOSE("#BTAG# Track association tool is not empty");
    //jetIsAssociated = m_BTagTrackAssocTool->BTagTrackAssociation_exec(&jets, h_BTaggingCollectionName.ptr());
    jetIsAssociated = m_BTagTrackAssocTool->BTagTrackAssociation_exec(h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr());
    if ( jetIsAssociated.isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to associate tracks to jet ");
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_WARNING("#BTAG# Empty track association tool ");
  }


  // Secondary vertex reconstruction.
  StatusCode SV = m_bTagSecVtxTool->BTagSecVtx_exec(h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr());
  if (SV.isFailure()) {
    ATH_MSG_WARNING("#BTAG# Failed to reconstruct sec vtx");
  }
 
  //Tag the jets
  SV = m_bTagTool->tagJet( h_JetCollectionName.ptr(), h_BTaggingCollectionName.ptr());
  if (SV.isFailure()) {
    ATH_MSG_WARNING("#BTAG# Failed in taggers call");
  } 

  //Create the element link from the jet to the btagging
  for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
    const xAOD::Jet * jetToTag = h_JetCollectionName->at(jetIndex); 
    xAOD::BTagging * itBTag = h_BTaggingCollectionName->at(jetIndex);
    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(*h_BTaggingCollectionName.ptr(), itBTag);
    h_jetBTaggingLinkName(*jetToTag) = linkBTagger;
  }

  return StatusCode::SUCCESS;

}


int JetBTaggerTool::modify(xAOD::JetContainer& jetsOriginal) const{

  // The procedure is slightly complicated: first the BTagging objects need to be created and ElementLinks
  // to them stored in the Jets, then the association can be done, and then finally the b-tagging proper
  // can be done


  //Force the jet container to be const to emulate ReadHandle
  xAOD::JetContainer const& jets = jetsOriginal;


  if (jets.size() == 0) {
    ATH_MSG_DEBUG("#BTAG# Empty JetContainer !!");
  }
  else {
    ATH_MSG_DEBUG("#BTAG#  Nb jets in JetContainer: "<< jets.size());
  }

  SG::WriteDecorHandle<xAOD::JetContainer,ElementLink< xAOD::BTaggingContainer > > h_jetBTaggingLinkName(m_jetBTaggingLinkName);

  //Create a xAOD::BTaggingContainer in any case (must be done) 
  std::string bTaggingContName = m_BTaggingCollectionName.key();
  ATH_MSG_DEBUG("#BTAG#  Container name: "<< bTaggingContName);

  /* Record the BTagging  output container */
  SG::WriteHandle<xAOD::BTaggingContainer> h_BTaggingCollectionName (m_BTaggingCollectionName);
  ATH_CHECK( h_BTaggingCollectionName.record(std::make_unique<xAOD::BTaggingContainer>(),
  					std::make_unique<xAOD::BTaggingAuxContainer>()) ); 
 
  xAOD::JetContainer::const_iterator itB = jets.begin();
  xAOD::JetContainer::const_iterator itE = jets.end();
  if (m_magFieldSvc->solenoidOn()) {
    for (xAOD::JetContainer::const_iterator it = itB ; it != itE; ++it) {
        xAOD::BTagging * newBTagMT  = new xAOD::BTagging();
        h_BTaggingCollectionName->push_back(newBTagMT);
    } //end loop JetContainer
  }
  else { //Solenoid OFF
    for (xAOD::JetContainer::const_iterator it = itB ; it != itE; ++it) {
      const xAOD::Jet& jetToTag = ( **it );
      ElementLink< xAOD::BTaggingContainer> linkBTagger;
      h_jetBTaggingLinkName(jetToTag) = linkBTagger;
    } //end loop JetContainer
    return StatusCode::SUCCESS;
  } //end test Solenoid status

  // We don't want to redo the track-jet association in case of augmentation; however, since
  // in the release-20 production the muon-jet association wasn't switched on and it is needed at
  // least for the JetVertexChargeTool, we do rerun it here. The working assumption is that in the
  // configuration, the track-jet association is switched off.
  StatusCode jetIsAssociated;
  if (!m_BTagTrackAssocTool.empty()) {
    ATH_MSG_VERBOSE("#BTAG# Track association tool is not empty");
    jetIsAssociated = m_BTagTrackAssocTool->BTagTrackAssociation_exec(&jets, h_BTaggingCollectionName.ptr());
    if ( jetIsAssociated.isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to associate tracks to jet ");
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_WARNING("#BTAG# Empty track association tool ");
  }

  // Secondary vertex reconstruction: unless it is clear that previous results are to be re-used, run this always.
  StatusCode SV = m_bTagSecVtxTool->BTagSecVtx_exec(&jets, h_BTaggingCollectionName.ptr());
  if (SV.isFailure()) {
    ATH_MSG_WARNING("#BTAG# Failed to reconstruct sec vtx");
  } 

  //Tag the jets
  SV = m_bTagTool->tagJet( &jets, h_BTaggingCollectionName.ptr());
  if (SV.isFailure()) {
    ATH_MSG_WARNING("#BTAG# Failed to tag the jets");
  } 

  //Create the element link from the jet to the btagging
  for (size_t jetIndex=0; jetIndex < jets.size() ; ++jetIndex) {
    const xAOD::Jet * jetToTag = jets.at(jetIndex); 
    xAOD::BTagging * itBTag = h_BTaggingCollectionName->at(jetIndex);
    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(*h_BTaggingCollectionName.ptr(), itBTag);
    h_jetBTaggingLinkName(*jetToTag) = linkBTagger;
  }

  return StatusCode::SUCCESS;
}

} //// namespace analysis
