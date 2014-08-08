/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/JetBTaggerTool.h"

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

#include <iostream>
#include <string>

namespace Analysis {


JetBTaggerTool::JetBTaggerTool(const std::string& n) : 
  asg::AsgTool(n),
  m_BTagName(""),
  m_BTagSVName(""),
  m_BTagJFVtxName(""),
  m_BTagTrackAssocTool("Analysis::BTagTrackAssociation"),
  m_bTagSecVtxTool("Analysis::BTagSecVertexing")
{

  declareProperty( "BTagTool", m_bTagTool);
  declareProperty( "BTagName", m_BTagName );
  declareProperty( "BTagTrackAssocTool", m_BTagTrackAssocTool);
  declareProperty( "BTagSVName", m_BTagSVName );
  declareProperty( "BTagJFVtxName", m_BTagJFVtxName );
  declareProperty( "BTagSecVertexing", m_bTagSecVtxTool);

}

JetBTaggerTool::~JetBTaggerTool()
{
  /////....
}


StatusCode JetBTaggerTool::initialize() {

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

  //retrive the bTagSecVtxTool
  if ( m_bTagSecVtxTool.retrieve().isFailure() ) {
    ATH_MSG_FATAL("#BTAGVTX# Failed to retrieve tool " << m_bTagSecVtxTool);
    return StatusCode::FAILURE;
  } else {
      ATH_MSG_DEBUG("#BTAGVTX# Retrieved tool " << m_bTagSecVtxTool); 
  }

  return StatusCode::SUCCESS;
}


int JetBTaggerTool::modify(xAOD::JetContainer& jets) const{

  // The procedure is slightly complicated: first the BTagging objects need to be created and ElementLinks
  // to them stored in the Jets, then the association can be done, and then finally the b-tagging proper
  // can be done

  if (jets.size() == 0) {
    ATH_MSG_DEBUG("#BTAG# Empty JetContainer !!");
    //ATH_MSG_WARNING("#BTAG# Empty JetContainer !!");
  }
  else {
    ATH_MSG_VERBOSE("#BTAG#  Nb jets in JetContainer: "<< jets.size());
    //ATH_MSG_WARNING("#BTAG#  Nb jets in JetContainer: "<< jets.size());
  }

  //Create a xAOD::BTaggingContainer in any case (must be done) 
  std::string bTaggingContName = m_BTagName;
 
  std::vector<xAOD::BTagging *> btagsList;
  xAOD::BTaggingContainer * bTaggingContainer;


  if (evtStore()->contains<xAOD::BTaggingContainer > ( bTaggingContName )) { //prepare re-tagging
    ATH_MSG_WARNING("#BTAG# BTagging container " << bTaggingContName << " in store, re-tagging? ");
    //retrieve it
    StatusCode sc  = evtStore()->retrieve(bTaggingContainer,bTaggingContName);
    if (sc.isFailure()) {
     ATH_MSG_WARNING("#BTAG# Failed to retrieve bTaggingContainer " << bTaggingContName);
    }
  }
  else {
    bTaggingContainer = new xAOD::BTaggingContainer();
    xAOD::BTaggingAuxContainer *bTaggingAuxContainer = new xAOD::BTaggingAuxContainer();
    CHECK( evtStore()->record(bTaggingAuxContainer, bTaggingContName+"Aux.") );
    bTaggingContainer->setStore(bTaggingAuxContainer);
    CHECK( evtStore()->record(bTaggingContainer, bTaggingContName) );
    ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " recorded in store");
  }
  //SV
  std::string bTagVertexContName = bTaggingContName + m_BTagSVName;
  xAOD::VertexContainer *bTagVertexContainer;
  if (evtStore()->contains<xAOD::VertexContainer > ( bTagVertexContName )) {
    ATH_MSG_WARNING("#BTAG# SV Vertex container " << bTagVertexContName );
    StatusCode sc  = evtStore()->retrieve(bTagVertexContainer,bTagVertexContName);
    if (sc.isFailure()) {
     ATH_MSG_WARNING("#BTAG# Failed to retrieve  bTagVertexContainer " <<bTagVertexContName );
    }
  }
  else {
    bTagVertexContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer *bTagVertexAuxContainer =  new xAOD::VertexAuxContainer();
    CHECK( evtStore()->record(bTagVertexAuxContainer, bTagVertexContName+"Aux.") );
    bTagVertexContainer->setStore(bTagVertexAuxContainer);
    CHECK( evtStore()->record(bTagVertexContainer, bTagVertexContName) );
    ATH_MSG_VERBOSE("#BTAG# SV Vertex container " << bTagVertexContName << " recorded in store");
  }          
 

  //JFVTX
  std::string bTagJFVertexContName = bTaggingContName + m_BTagJFVtxName;
  xAOD::BTagVertexContainer *bTagJFVertexContainer;
  if (evtStore()->contains<xAOD::BTagVertexContainer > ( bTagJFVertexContName )) {
    ATH_MSG_WARNING("#BTAG# JetFitter Vertex container " << bTagJFVertexContName );
    StatusCode sc  = evtStore()->retrieve(bTagJFVertexContainer,bTagJFVertexContName);
    if (sc.isFailure()) {
     ATH_MSG_WARNING("#BTAG# Failed to retrieve  bTagJFVertexContainer " << bTagJFVertexContName);
    }
  }
  else {
    bTagJFVertexContainer = new xAOD::BTagVertexContainer();
    xAOD::BTagVertexAuxContainer *bTagJFVertexAuxContainer =  new xAOD::BTagVertexAuxContainer();
    CHECK( evtStore()->record(bTagJFVertexAuxContainer, bTagJFVertexContName+"Aux.") );
    bTagJFVertexContainer->setStore(bTagJFVertexAuxContainer);
    CHECK( evtStore()->record(bTagJFVertexContainer, bTagJFVertexContName) );
    ATH_MSG_VERBOSE("#BTAG# JetFitter Vertex container " << bTagJFVertexContName << " recorded in store");
  }      

  


  std::vector<xAOD::Jet*> jetsList;
  xAOD::JetContainer::iterator itB = jets.begin();
  xAOD::JetContainer::iterator itE = jets.end();
  for(xAOD::JetContainer::iterator it = itB ; it != itE; ++it) {

    xAOD::Jet& jetToTag = ( **it );
    jetsList.push_back(&jetToTag);

    xAOD::BTagging * newBTag = new xAOD::BTagging();

    //Push the BTagging object in the container and in the temporary vector
    bTaggingContainer->push_back(newBTag);
    //newBTag->auxdata<float>("TestBTag")=1;
    btagsList.push_back(newBTag);
    //Create an element link to be passed to the tagged Jet.
    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(*bTaggingContainer, newBTag);
    jetToTag.setBTaggingLink(linkBTagger);
  } //end loop JetContainer
  
  //return StatusCode::SUCCESS;  

  StatusCode jetIsAssociated;
  if (!m_BTagTrackAssocTool.empty()) {
    ATH_MSG_VERBOSE("#BTAG# Track association tool is not empty");
    jetIsAssociated = m_BTagTrackAssocTool->BTagTrackAssociation_exec(&jetsList);
  }
  else {
    ATH_MSG_WARNING("#BTAG# Empty track association tool ");
  }
  if ( jetIsAssociated.isFailure() ) {
    ATH_MSG_ERROR("#BTAG# Failed to associate tracks to jet ");
    return StatusCode::FAILURE;
  }
  
  std::vector<xAOD::BTagging *>::iterator itBTag = btagsList.begin();
  //  
  for(xAOD::JetContainer::iterator it = itB ; it != itE; ++it,++itBTag) {
    xAOD::Jet& jetToTag = ( **it );
    //SV
    StatusCode SV = m_bTagSecVtxTool->BTagSecVtx_exec(jetToTag, *itBTag, bTagVertexContainer, bTagJFVertexContainer);
    if (SV.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed to reconstruct sec vtx");
    }

    StatusCode sc = m_bTagTool->tagJet( jetToTag, *itBTag );
    if (sc.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed in taggers call");
    }
   }


   // if ( bTaggingContainer->size() > 0) {
   //   CHECK( evtStore()->record(bTaggingContainer, jetcollname+"_btagInfo") );
   // }
    

  /// testme
  /*for(xAOD::JetContainer::iterator it = itB ; it != itE; ++it) {
    xAOD::Jet& jetToTag = ( **it );

    const xAOD::BTagging* testbtag = jetToTag.getBTagging();
    std::cout << "georges say sv0 = " << testbtag->SV0_significance3D() << std::endl;

  }*/


  return 1;

}


} //// namespace analysis
