/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/JetBTaggerTool.h"

#include "xAODCore/ShallowCopy.h"
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
  m_bTagSecVtxTool("Analysis::BTagSecVertexing"),
  m_retag(false)
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
  }
  else {
    ATH_MSG_VERBOSE("#BTAG#  Nb jets in JetContainer: "<< jets.size());
  }

  //Create a xAOD::BTaggingContainer in any case (must be done) 
  std::string bTaggingContName = m_BTagName;
 
  std::vector<xAOD::BTagging *> btagsList;
  xAOD::BTaggingContainer * bTaggingContainer(0);
  xAOD::BTaggingAuxContainer * bTaggingAuxContainer(0);

  
  if (evtStore()->contains<xAOD::BTaggingContainer > ( bTaggingContName )) { //prepare re-tagging
    //BTaggingContainer in SG - overwrite it
    ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " in store, re-tagging scenario");
    m_retag = true;
    bTaggingContainer = new xAOD::BTaggingContainer();
    bTaggingAuxContainer = new xAOD::BTaggingAuxContainer();
    CHECK( evtStore()->overwrite(bTaggingAuxContainer, bTaggingContName+"Aux.") );
    bTaggingContainer->setStore(bTaggingAuxContainer);
    CHECK( evtStore()->overwrite(bTaggingContainer, bTaggingContName) );
    ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " overwritten in store");
  }
  else {
    if (!m_retag) {
      //No BTaggingContainer not in SG and not re-tagging - record it in SG
      ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " not in store, Jet reco scenario");
      bTaggingContainer = new xAOD::BTaggingContainer();
      bTaggingAuxContainer = new xAOD::BTaggingAuxContainer();
      CHECK( evtStore()->record(bTaggingAuxContainer, bTaggingContName+"Aux.") );
      bTaggingContainer->setStore(bTaggingAuxContainer);
      CHECK( evtStore()->record(bTaggingContainer, bTaggingContName) );
      ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " recorded in store");
    }
  }

  // The SV need to be remade in case of re-tagging, or simply used if b-tagging information is merely to be extended.
  // If the SV container does not exist it is created.
  // It is conceivable that they do not exist but also don't need to be created anymore (e.g. if
  // the container was slimmed away but is also not needed anymore), but we will ignore this case.
  std::string bTagSecVertexContName = bTaggingContName + m_BTagSVName;
  xAOD::VertexContainer *bTagSecVertexContainer(0);
  xAOD::ShallowAuxContainer* bTagSVShallowAuxContainer(0);
  bool reuse_SVContainer = false;
  if (evtStore()->contains<xAOD::VertexContainer > ( bTagSecVertexContName )) {
    if (m_retag) {
      bTagSecVertexContainer = new xAOD::VertexContainer;
      xAOD::VertexAuxContainer * bTagSecVertexAuxContainer =  new xAOD::VertexAuxContainer;
      CHECK( evtStore()->overwrite(bTagSecVertexAuxContainer, bTagSecVertexContName+"Aux.") );
      bTagSecVertexContainer->setStore(bTagSecVertexAuxContainer);
      CHECK( evtStore()->overwrite(bTagSecVertexContainer, bTagSecVertexContName) );
      ATH_MSG_VERBOSE("#BTAG# SV container " << bTagSecVertexContName << " overwritten in store");
    } else {
      const xAOD::VertexContainer *bTagSecVertexContainerAOD;
      StatusCode sc  = evtStore()->retrieve(bTagSecVertexContainerAOD,bTagSecVertexContName);
      if (sc.isFailure()) {
       ATH_MSG_WARNING("#BTAG# Failed to retrieve  bTagSecVertexContainer " <<bTagSecVertexContName );
      } else {
	auto rec = xAOD::shallowCopyContainer (*bTagSecVertexContainerAOD);
	bTagSecVertexContainer = rec.first;
        bTagSVShallowAuxContainer = rec.second;
	reuse_SVContainer = true;
      }
    }
  } else {
    bTagSecVertexContainer = new xAOD::VertexContainer();
    xAOD::VertexAuxContainer * bTagSecVertexAuxContainer =  new xAOD::VertexAuxContainer();
    CHECK( evtStore()->record(bTagSecVertexAuxContainer, bTagSecVertexContName+"Aux.") );
    bTagSecVertexContainer->setStore(bTagSecVertexAuxContainer);
    CHECK( evtStore()->record(bTagSecVertexContainer, bTagSecVertexContName) );
    ATH_MSG_VERBOSE("#BTAG# SV Vertex container " << bTagSecVertexContName << " recorded in store");
  }

  // JFVertex container: the logic here is the same as for the "ordinary" SV container.
  std::string bTagJFVertexContName = bTaggingContName + m_BTagJFVtxName;
  xAOD::BTagVertexContainer *bTagJFVertexContainer(0);
  xAOD::ShallowAuxContainer* bTagJFVShallowAuxContainer(0);
  bool reuse_JFVContainer = false;
  if (evtStore()->contains<xAOD::BTagVertexContainer > ( bTagJFVertexContName )) {
    if (m_retag) {
      bTagJFVertexContainer = new xAOD::BTagVertexContainer();
      xAOD::BTagVertexAuxContainer * bTagJFVertexAuxContainer =  new xAOD::BTagVertexAuxContainer();
      CHECK( evtStore()->overwrite(bTagJFVertexAuxContainer, bTagJFVertexContName+"Aux.") );
      bTagJFVertexContainer->setStore(bTagJFVertexAuxContainer);
      CHECK( evtStore()->overwrite(bTagJFVertexContainer, bTagJFVertexContName) );
      ATH_MSG_VERBOSE("#BTAG# JetFitter Vertex container " << bTagJFVertexContName << " overwritten in store");
    } else {
      const xAOD::BTagVertexContainer *bTagJFVertexContainerAOD;
      StatusCode sc  = evtStore()->retrieve(bTagJFVertexContainerAOD,bTagJFVertexContName);
      if (sc.isFailure()) {
	ATH_MSG_WARNING("#BTAG# Failed to retrieve  bTagJFVertexContainer " <<bTagJFVertexContName );
      } else {
	auto rec = xAOD::shallowCopyContainer (*bTagJFVertexContainerAOD);
	bTagJFVertexContainer = rec.first;
        bTagJFVShallowAuxContainer = rec.second;
	reuse_JFVContainer = true;
      }
    }
  } else {
    bTagJFVertexContainer = new xAOD::BTagVertexContainer();
    xAOD::BTagVertexAuxContainer *bTagJFVertexAuxContainer =  new xAOD::BTagVertexAuxContainer();
    CHECK( evtStore()->record(bTagJFVertexAuxContainer, bTagJFVertexContName+"Aux.") );
    bTagJFVertexContainer->setStore(bTagJFVertexAuxContainer);
    CHECK( evtStore()->record(bTagJFVertexContainer, bTagJFVertexContName) );
    ATH_MSG_VERBOSE("#BTAG# JetFitter Vertex container " << bTagJFVertexContName << " recorded in store");
  }

  // Since the secondary vertex reconstruction deals with the "ordinary" SV and JetFitter SV simultaneously,
  // they should always be handled together. Since the logic above allows for differences between the two,
  // these are at least flagged explicitly here.
  if ((reuse_SVContainer && ! reuse_JFVContainer) || (! reuse_SVContainer && reuse_JFVContainer)) {
    ATH_MSG_WARNING("#BTAG# Inconsistent results obtained for SV and JetFitter vertex containers will lead to inconsistent ElementLinks to SV/JFV in output");
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
    btagsList.push_back(newBTag);
    if (!m_retag) {
      //Create an element link to be passed to the tagged Jet.
      //Nothing done in case of re-tagging, assuming the same order is used
      ElementLink< xAOD::BTaggingContainer> linkBTagger;
      linkBTagger.toContainedElement(*bTaggingContainer, newBTag);
      jetToTag.setBTaggingLink(linkBTagger);
    }
  } //end loop JetContainer

  //if (!m_retag) {
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
  //}

  std::vector<xAOD::BTagging *>::iterator itBTag = btagsList.begin();
  for(xAOD::JetContainer::iterator it = itB ; it != itE; ++it,++itBTag) {
    xAOD::Jet& jetToTag = ( **it );
    // Secondary vertex reconstruction: unless it is clear that previous results are to be re-used, run this always.
    if (! reuse_SVContainer) {
      StatusCode SV = m_bTagSecVtxTool->BTagSecVtx_exec(jetToTag, *itBTag, bTagSecVertexContainer, bTagJFVertexContainer);
      if (SV.isFailure()) {
	ATH_MSG_WARNING("#BTAG# Failed to reconstruct sec vtx");
      }
    }
    StatusCode sc = m_bTagTool->tagJet( jetToTag, *itBTag );
    if (sc.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed in taggers call");
    }
  }

   if (reuse_SVContainer) { //is a shallow copy
     delete bTagSecVertexContainer;
     delete bTagSVShallowAuxContainer;
   }

   if (reuse_JFVContainer) { //is a shallow copy
     delete bTagJFVertexContainer;
     delete bTagJFVShallowAuxContainer;
   }

  /// testme
  /*for(xAOD::JetContainer::iterator it = itB ; it != itE; ++it) {
    xAOD::Jet& jetToTag = ( **it );

    const xAOD::BTagging* testbtag = jetToTag.getBTagging();
    std::cout << "georges say sv0 = " << testbtag->SV0_significance3D() << std::endl;

  }*/

  return 1;

}


} //// namespace analysis
