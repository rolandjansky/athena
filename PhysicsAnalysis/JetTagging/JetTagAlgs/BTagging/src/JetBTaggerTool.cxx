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
    ATH_MSG_VERBOSE("#BTAG#  Nb jets in JetContainer: "<< h_JetCollectionName->size());
    ATH_MSG_INFO("#BTAG#  Nb jets in JetContainer: "<< h_JetCollectionName->size());
  }

  //Decor Jet with element link to the BTagging
  SG::WriteDecorHandle<xAOD::JetContainer,ElementLink< xAOD::BTaggingContainer > > h_jetBTaggingLinkName(m_jetBTaggingLinkName);

  //Create a xAOD::BTaggingContainer in any case (must be done) 
  std::string bTaggingContName = m_BTaggingCollectionName.key();
  ATH_MSG_INFO("#BTAG#  Container name: "<< bTaggingContName);

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
    ATH_MSG_INFO("#BTAG# Track association tool is not empty");
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
  for (size_t jetIndex=0; jetIndex < h_JetCollectionName->size() ; ++jetIndex) {
    xAOD::Jet&  jetToTag = const_cast<xAOD::Jet&>(*h_JetCollectionName->at(jetIndex));
    xAOD::BTagging * itBTag = h_BTaggingCollectionName->at(jetIndex);
    StatusCode sc = m_bTagTool->tagJet( jetToTag, itBTag );
    if (sc.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed in taggers call");
    }
    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(*h_BTaggingCollectionName.ptr(), itBTag);
    h_jetBTaggingLinkName(jetToTag) = linkBTagger;
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
    ATH_MSG_VERBOSE("#BTAG#  Nb jets in JetContainer: "<< jets.size());
    ATH_MSG_INFO("#BTAG#  Nb jets in JetContainer: "<< jets.size());
  }

  SG::WriteDecorHandle<xAOD::JetContainer,ElementLink< xAOD::BTaggingContainer > > h_jetBTaggingLinkName(m_jetBTaggingLinkName);

  //Create a xAOD::BTaggingContainer in any case (must be done) 
  std::string bTaggingContName = m_BTaggingCollectionName.key();
  ATH_MSG_INFO("#BTAG#  Container name: "<< bTaggingContName);

  /* Record the BTagging  output container */
  SG::WriteHandle<xAOD::BTaggingContainer> h_BTaggingCollectionName (m_BTaggingCollectionName);
  ATH_CHECK( h_BTaggingCollectionName.record(std::make_unique<xAOD::BTaggingContainer>(),
  					std::make_unique<xAOD::BTaggingAuxContainer>()) ); 
 
  //xAOD::BTaggingContainer * bTaggingContainer(0);

  // Keep track (on an event by event basis; not sure whether this is really necessary) of whether Flavour
  // Tagging information exists already or not, as in that case it needs to be overwritten rather than created.
  bool retag = false;
  
  /*if (evtStore()->contains<xAOD::BTaggingContainer > ( bTaggingContName )) { //prepare re-tagging
    //BTaggingContainer in SG - overwrite it (copying BTagging objects if in "augmentation" mode)
    ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " in store, re-tagging scenario");
    retag = true;

    StatusCode sc = overwrite<xAOD::BTaggingContainer,xAOD::BTaggingAuxContainer>(bTaggingContName, m_augment);
    if (sc.isFailure()) return sc;
    CHECK( evtStore()->retrieve(bTaggingContainer, bTaggingContName) );
    if (m_augment && jets.size() != bTaggingContainer->size() && m_magFieldSvc->solenoidOn()) {
      ATH_MSG_ERROR("#BTAG# existing BTaggingContainer has size" << bTaggingContainer->size() << ", expected " << jets.size());
    }
    for (const xAOD::Jet* j : jets) {
      const_cast<ElementLink< xAOD::BTaggingContainer >&>(j->btaggingLink()).toTransient();
    }
  }
  else {
    if (m_augment)
      ATH_MSG_WARNING("#BTAG# augmentation requested for non-existent BTaggingContainer");
    //No BTaggingContainer not in SG and not re-tagging - record it in SG
    ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " not in store, Jet reco scenario");
    bTaggingContainer = new xAOD::BTaggingContainer();
    xAOD::BTaggingAuxContainer* bTaggingAuxContainer = new xAOD::BTaggingAuxContainer();
    CHECK( evtStore()->record(bTaggingAuxContainer, bTaggingContName+"Aux.") );
    bTaggingContainer->setStore(bTaggingAuxContainer);
    CHECK( evtStore()->record(bTaggingContainer, bTaggingContName) );
    ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " recorded in store");
  }*/


  // The SV need to be remade in case of re-tagging, or simply used if b-tagging information is merely to be extended.
  // If the SV container does not exist it is created.
  // It is conceivable that they do not exist but also don't need to be created anymore (e.g. if
  // the container was slimmed away but is also not needed anymore), but we will ignore this case.
  //std::string bTagSecVertexContName = bTaggingContName + m_BTagSVCollectionName.key();
  /*xAOD::VertexContainer *bTagSecVertexContainer(0);
  xAOD::ShallowAuxContainer* bTagSVShallowAuxContainer(0);
  bool reuse_SVContainer = false;
  if (evtStore()->contains<xAOD::VertexContainer > ( bTagSecVertexContName )) {
    if (retag && !m_augment) {
      StatusCode sc = overwrite<xAOD::VertexContainer,xAOD::VertexAuxContainer>(bTagSecVertexContName, false);
      if (sc.isFailure()) return sc;
      CHECK( evtStore()->retrieve(bTagSecVertexContainer, bTagSecVertexContName) );
      ATH_MSG_VERBOSE("#BTAG# SV container " << bTagSecVertexContName << " overwritten in store");
    } else {
      const xAOD::VertexContainer *bTagSecVertexContainerAOD;
      StatusCode sc  = evtStore()->retrieve(bTagSecVertexContainerAOD,bTagSecVertexContName);
      if (sc.isFailure()) {
       ATH_MSG_WARNING("#BTAG# Failed to retrieve bTagSecVertexContainer " << bTagSecVertexContName );
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
  }*/

  // JFVertex container: the logic here is the same as for the "ordinary" SV container.
  //std::string bTagJFVertexContName = bTaggingContName + m_BTagJFVtxCollectionName.key();
  /*xAOD::BTagVertexContainer *bTagJFVertexContainer(0);
  xAOD::ShallowAuxContainer* bTagJFVShallowAuxContainer(0);
  bool reuse_JFVContainer = false;
  if (evtStore()->contains<xAOD::BTagVertexContainer > ( bTagJFVertexContName )) {
    if (retag && !m_augment) {
      StatusCode sc = overwrite<xAOD::BTagVertexContainer,xAOD::BTagVertexAuxContainer>(bTagJFVertexContName, false);
      if (sc.isFailure()) return sc;
      CHECK( evtStore()->retrieve(bTagJFVertexContainer, bTagJFVertexContName) );
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
  }*/

  // Since the secondary vertex reconstruction deals with the "ordinary" SV and JetFitter SV simultaneously,
  // they should always be handled together. Since the logic above allows for differences between the two,
  // these are at least flagged explicitly here.
/*  if ((reuse_SVContainer && ! reuse_JFVContainer) || (! reuse_SVContainer && reuse_JFVContainer)) {
    ATH_MSG_WARNING("#BTAG# Inconsistent results obtained for SV and JetFitter vertex containers will lead to inconsistent ElementLinks to SV/JFV in output");
  }
*/
   
  //ATH_MSG_INFO("MANU before creating BTagging object");
  //std::vector<xAOD::Jet*> jetsList;
  xAOD::JetContainer::const_iterator itB = jets.begin();
  xAOD::JetContainer::const_iterator itE = jets.end();
  if (m_magFieldSvc->solenoidOn()) {
    // In case of augmentation, fill the btagsList vector here (so as not to complicate the downstream logic)
    /*if (m_augment) {
      for (auto bt : *bTaggingContainer) btagsList.push_back(bt);
    }*/
    //unsigned int ibtag = 0;
    for (xAOD::JetContainer::const_iterator it = itB ; it != itE; ++it) {
      //xAOD::Jet& jetToTag = ( **it );
      //jetsList.push_back(&jetToTag);
      // In case of augmentation, the BTagging object has been copied and added to btagsList already.
      /*xAOD::BTagging* newBTag = 0;
      if (m_augment) {
	newBTag = btagsList[ibtag++];
      } else {*/
        xAOD::BTagging * newBTagMT  = new xAOD::BTagging();
        h_BTaggingCollectionName->push_back(newBTagMT);
        //std::unique_ptr<xAOD::BTagging> newBTagMT  = std::make_unique<xAOD::BTagging>();
        //h_BTaggingCollectionName->push_back(std::move(newBTagMT));
	//newBTag = new xAOD::BTagging();
        //bTaggingContainer = new xAOD::BTaggingContainer();
	//bTaggingContainer->push_back(newBTag);
	//xAOD::BTagging * mylastBTag = h_BTaggingCollectionName->back();
        //ATH_MSG_INFO("MANU before test SetV0");
        //double sv0_significance3D = gRandom->Gaus(0.,1);
        //mylastBTag->setSV0_significance3D(sv0_significance3D);
        //newBTagMT->setSV0_significance3D(sv0_significance3D);
        //ATH_MSG_INFO("MANU before test DL1");
        //mylastBTag->setVariable<double>("DL1","pu",1.1);
        //ATH_MSG_INFO("MANU before test int");
        //mylastBTag->auxdata<int >("TestINT") =1;
        //ATH_MSG_INFO("MANU after test int");

	//newBTag = new xAOD::BTagging();
	//Push the BTagging object in the container and in the temporary vector
	//bTaggingContainer->push_back(newBTag);
	//btagsList.push_back(mylastBTag);
      //}
      /*if (!retag) {
        //Create an element link to be passed to the tagged Jet.
        //Nothing done in case of re-tagging, assuming the same order is used
        ElementLink< xAOD::BTaggingContainer> linkBTagger;
        linkBTagger.toContainedElement(*bTaggingContainer, newBTag);
        jetToTag.setBTaggingLink(linkBTagger);
      }*/
    } //end loop JetContainer
  }
  else { //Solenoid OFF
    for (xAOD::JetContainer::const_iterator it = itB ; it != itE; ++it) {
      const xAOD::Jet& jetToTag = ( **it );
      if (!retag) {
	ElementLink< xAOD::BTaggingContainer> linkBTagger;
        h_jetBTaggingLinkName(jetToTag) = linkBTagger;
        //jetToTag.setBTaggingLink(linkBTagger);
      }
    } //end loop JetContainer
  } //end test Solenoid status


  // From here on, no modification should be made to the JetContainer anymore.
  // We use this to create a shallow copy of the JetContainer, for the case of track-jet collections.
  // This allows us to rescale the jet pt values used in the Flavour Tagging algorithms without affecting the input jets.
  
/*  xAOD::ShallowAuxContainer* jetShallowAuxContainer(0);
  xAOD::JetContainer* jetShallowContainer(0);
  if (m_magFieldSvc->solenoidOn()) {
    if (m_PtRescale) {
      auto rec = xAOD::shallowCopyContainer (jets);
      jetShallowContainer = rec.first;
      jetShallowAuxContainer = rec.second;
      StatusCode sc = m_PtRescalingTool->BTagJetPtScaling_exec(*jetShallowContainer);
      if (sc.isFailure()) {
        ATH_MSG_WARNING("#BTAG# Failed to carry out jet pt rescaling");
      }
      // also change the iterators to point to the shallow-copied JetContainer
      itB = jetShallowContainer->begin();
      itE = jetShallowContainer->end();
      // and modify the jetsList variable accordingly
      unsigned int ijet = 0;
      for (auto it = itB; it != itE; ++it) {
        xAOD::Jet& jetToTag = **it;
        jetsList[ijet++] = &jetToTag;
      }
    } 
  } 
  */
  
  // if (!m_augment) {
  // We don't want to redo the track-jet association in case of augmentation; however, since
  // in the release-20 production the muon-jet association wasn't switched on and it is needed at
  // least for the JetVertexChargeTool, we do rerun it here. The working assumption is that in the
  // configuration, the track-jet association is switched off.
  StatusCode jetIsAssociated;
  if (!m_BTagTrackAssocTool.empty()) {
    ATH_MSG_VERBOSE("#BTAG# Track association tool is not empty");
    ATH_MSG_INFO("#BTAG# Track association tool is not empty");
    jetIsAssociated = m_BTagTrackAssocTool->BTagTrackAssociation_exec(&jets, h_BTaggingCollectionName.ptr());
    if ( jetIsAssociated.isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to associate tracks to jet ");
      return StatusCode::FAILURE;
    }
  }
  else {
    ATH_MSG_WARNING("#BTAG# Empty track association tool ");
  }
  // }

  // Secondary vertex reconstruction: unless it is clear that previous results are to be re-used, run this always.
  //if (! (reuse_SVContainer || m_augment)) {
  StatusCode SV = m_bTagSecVtxTool->BTagSecVtx_exec(&jets, h_BTaggingCollectionName.ptr());
  if (SV.isFailure()) {
    ATH_MSG_WARNING("#BTAG# Failed to reconstruct sec vtx");
  } 

  //Tag the jets
  SV = m_bTagTool->tagJet( &jets, h_BTaggingCollectionName.ptr());
  if (SV.isFailure()) {
    ATH_MSG_WARNING("#BTAG# Failed in taggers call");
  } 

  //Create the element link from the jet to the btagging
  for (size_t jetIndex=0; jetIndex < jets.size() ; ++jetIndex) {
    xAOD::Jet&  jetToTag = const_cast<xAOD::Jet&>(*jets.at(jetIndex)); 
    xAOD::BTagging * itBTag = h_BTaggingCollectionName->at(jetIndex);
    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(*h_BTaggingCollectionName.ptr(), itBTag);
    h_jetBTaggingLinkName(jetToTag) = linkBTagger;
    //jetToTag.setBTaggingLink(linkBTagger);
  }

/*  std::vector<xAOD::BTagging *>::iterator itBTag = btagsList.begin();
  std::vector<xAOD::Jet *>::iterator itJetB = jetsList.begin();
  std::vector<xAOD::Jet *>::iterator itJetE = jetsList.end();
  for (std::vector<xAOD::Jet *>::iterator it = itJetB ; it != itJetE; ++it,++itBTag) {
    xAOD::Jet& jetToTag = ( **it );
    StatusCode sc = m_bTagTool->tagJet( jetToTag, *itBTag );
    if (sc.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed in taggers call");
    }
    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(h_BTaggingCollectionName.ptr(), itBTag);
    if (sc.isFailure()) {
      ATH_MSG_WARNING("#BTAG# Failed in taggers call");
    }
    ElementLink< xAOD::BTaggingContainer> linkBTagger;
    linkBTagger.toContainedElement(h_BTaggingCollectionName.ptr(), itBTag);
    jetToTag.setBTaggingLink(linkBTagger);
    jetToTag.setBTaggingLink(linkBTagger);
  }
*/
/*   if (reuse_SVContainer) { //is a shallow copy
     delete bTagSecVertexContainer;
     delete bTagSVShallowAuxContainer;
   }

   if (reuse_JFVContainer) { //is a shallow copy
     delete bTagJFVertexContainer;
     delete bTagJFVShallowAuxContainer;
   }
*/
   /*if (m_PtRescale) { // is a shallow copy
     delete jetShallowAuxContainer;
     delete jetShallowContainer;
   }*/
   
  /// testme
  /*for(xAOD::JetContainer::iterator it = itB ; it != itE; ++it) {
    xAOD::Jet& jetToTag = ( **it );

    const xAOD::BTagging* testbtag = jetToTag.getBTagging();
    std::cout << "georges say sv0 = " << testbtag->SV0_significance3D() << std::endl;

  }*/

  return 1;

}

  // Since we are having to do some manipulations for multiple containers, use a few functions to take care of this.
  // We keep the functions private so that no other compilation unit can try to 
  // Code "borrowed" from IsolationAugmentationToolMuons.cxx_nocompile and modified.
  // This functionality is needed when overwriting containers within the Derivation context.
  
  template< class CONTAINER, class AUXSTORE >
  StatusCode
  JetBTaggerTool::overwrite(const std::string& key, bool doCopy) const {
   
    // Let the user know what's happening:
    ATH_MSG_VERBOSE( "Running deepCopy on container: " << key );
   
    // Decide which implementation to call:
    if (evtStore()->template contains< AUXSTORE >(key+"Aux.")) {
      if (overwriteImp< CONTAINER, AUXSTORE >(key, doCopy).isFailure()) {
        ATH_MSG_FATAL( "Couldn't call overwriteImp with concrete auxiliary store" );
        return StatusCode::FAILURE;
      }
    } else if (evtStore()->template contains< xAOD::AuxContainerBase >(key+"Aux.")) {
      if (overwriteImp<CONTAINER, xAOD::AuxContainerBase>(key, doCopy).isFailure()) {
        ATH_MSG_FATAL( "Couldn't call overwriteImp with generic auxiliary store" );
        return StatusCode::FAILURE;
      }
    } else {
      ATH_MSG_FATAL( "Couldn't discover auxiliary store type for container \"" << key << "\"" );
      return StatusCode::FAILURE;
    }
   
    // Return gracefully:
    return StatusCode::SUCCESS;
  }

  template< class CONTAINER, class AUXSTORE >
  StatusCode
  JetBTaggerTool::overwriteImp(const std::string& key, bool doCopy) const {
   
    // Retrieve the const container:
    const CONTAINER* c = 0;
    ATH_CHECK( evtStore()->retrieve(c, key) );
   
    // Create the new container:
    CONTAINER* copy = new CONTAINER();
    AUXSTORE* copyAux = new AUXSTORE();
    copy->setStore(copyAux);

    if (doCopy) {
      // Create the new objects, doing a deep copy if requested
      for (auto oldObj : *c ) {
	auto newObj = new typename CONTAINER::base_value_type();
	copy->push_back(newObj);
	*newObj = *oldObj;
      }
    }
   
    // Do the overwrite:
    ATH_CHECK( evtStore()->overwrite( copy, key, true, false ) );
    ATH_CHECK( evtStore()->overwrite( copyAux, key + "Aux.", true, false ) );
   
    // Return gracefully:
    return StatusCode::SUCCESS;
  }



} //// namespace analysis
