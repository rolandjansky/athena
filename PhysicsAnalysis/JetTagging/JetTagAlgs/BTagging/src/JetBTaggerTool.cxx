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
#include "BTagging/BTagJetPtScaling.h"
#include "JetTagTools/JetTagUtils.h"

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
  m_PtRescalingTool("Analysis::BTagJetPtScaling"),
  m_augment(false),
  m_PtRescale(false),
  m_magFieldSvc("AtlasFieldSvc",n)
{

  declareProperty( "BTagTool", m_bTagTool);
  declareProperty( "BTagName", m_BTagName );
  declareProperty( "BTagTrackAssocTool", m_BTagTrackAssocTool);
  declareProperty( "BTagSVName", m_BTagSVName );
  declareProperty( "BTagJFVtxName", m_BTagJFVtxName );
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

  //modify can be called twice by standalone btagging algorithm for PFlow jets, the first one is tagged with EMTopo calibration (keeping same name as before)
  //the second one is tagged with PFlow calibration ("_PFlowTune" added to the names of all containers)
  bool pflow = false;
  if (evtStore()->contains<xAOD::BTaggingContainer > ( bTaggingContName )) {
    if (jets.size() > 0) {
      xAOD::JetContainer::iterator itB = jets.begin();
      xAOD::Jet& jet = **itB;
      std::string author = JetTagUtils::getJetAuthor(jet);
      if (author == "AntiKt4EMPFlow") {
        //already one AntiKt4EMPFlow tagged, the second one is the self tune one's
        ATH_MSG_DEBUG("#BTAG# BTagging container " << bTaggingContName << " in store, pflow tune scenario");
        bTaggingContName += "_PFlowTune";
        pflow = true;
      }
    }
  } 
 
  std::vector<xAOD::BTagging *> btagsList;
  xAOD::BTaggingContainer * bTaggingContainer(0);
  // xAOD::BTaggingAuxContainer * bTaggingAuxContainer(0);

  // Keep track (on an event by event basis; not sure whether this is really necessary) of whether Flavour
  // Tagging information exists already or not, as in that case it needs to be overwritten rather than created.
  bool retag = false;
  
  if (evtStore()->contains<xAOD::BTaggingContainer > ( bTaggingContName ) && !pflow) { //prepare re-tagging
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
    ATH_MSG_WARNING("#BTAG# BTagging container " << bTaggingContName << " not in store, Jet reco scenario");
    bTaggingContainer = new xAOD::BTaggingContainer();
    xAOD::BTaggingAuxContainer* bTaggingAuxContainer = new xAOD::BTaggingAuxContainer();
    CHECK( evtStore()->record(bTaggingAuxContainer, bTaggingContName+"Aux.") );
    bTaggingContainer->setStore(bTaggingAuxContainer);
    CHECK( evtStore()->record(bTaggingContainer, bTaggingContName) );
    ATH_MSG_VERBOSE("#BTAG# BTagging container " << bTaggingContName << " recorded in store");
    ATH_MSG_WARNING("#BTAG# BTagging container " << bTaggingContName << " recorded in store");
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
  }

  // JFVertex container: the logic here is the same as for the "ordinary" SV container.
  std::string bTagJFVertexContName = bTaggingContName + m_BTagJFVtxName;
  xAOD::BTagVertexContainer *bTagJFVertexContainer(0);
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
  if (m_magFieldSvc->solenoidOn()) {
    // In case of augmentation, fill the btagsList vector here (so as not to complicate the downstream logic)
    if (m_augment) {
      for (auto bt : *bTaggingContainer) btagsList.push_back(bt);
    }
    unsigned int ibtag = 0;
    for (xAOD::JetContainer::iterator it = itB ; it != itE; ++it) {
      xAOD::Jet& jetToTag = ( **it );
      jetsList.push_back(&jetToTag);
      // In case of augmentation, the BTagging object has been copied and added to btagsList already.
      xAOD::BTagging* newBTag = 0;
      if (m_augment) {
	newBTag = btagsList[ibtag++];
      } else {
	newBTag = new xAOD::BTagging();
	//Push the BTagging object in the container and in the temporary vector
	bTaggingContainer->push_back(newBTag);
	btagsList.push_back(newBTag);
      }
      if (!retag) {
        //Create an element link to be passed to the tagged Jet.
        //Nothing done in case of re-tagging, assuming the same order is used
        ElementLink< xAOD::BTaggingContainer> linkBTagger;
        linkBTagger.toContainedElement(*bTaggingContainer, newBTag);
        jetToTag.setBTaggingLink(linkBTagger);
      }
    } //end loop JetContainer
  }
  else { //Solenoid OFF
    for (xAOD::JetContainer::iterator it = itB ; it != itE; ++it) {
      xAOD::Jet& jetToTag = ( **it );
      if (!retag) {
	ElementLink< xAOD::BTaggingContainer> linkBTagger;
        jetToTag.setBTaggingLink(linkBTagger);
      }
    } //end loop JetContainer
  } //end test Solenoid status


  // From here on, no modification should be made to the JetContainer anymore.
  // We use this to create a shallow copy of the JetContainer, for the case of track-jet collections.
  // This allows us to rescale the jet pt values used in the Flavour Tagging algorithms without affecting the input jets.
  
  xAOD::ShallowAuxContainer* jetShallowAuxContainer(0);
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
  
  // if (!m_augment) {
  // We don't want to redo the track-jet association in case of augmentation; however, since
  // in the release-20 production the muon-jet association wasn't switched on and it is needed at
  // least for the JetVertexChargeTool, we do rerun it here. The working assumption is that in the
  // configuration, the track-jet association is switched off.
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
  // }

  std::vector<xAOD::BTagging *>::iterator itBTag = btagsList.begin();
  std::vector<xAOD::Jet *>::iterator itJetB = jetsList.begin();
  std::vector<xAOD::Jet *>::iterator itJetE = jetsList.end();
  for (std::vector<xAOD::Jet *>::iterator it = itJetB ; it != itJetE; ++it,++itBTag) {
    xAOD::Jet& jetToTag = ( **it );
    // Secondary vertex reconstruction: unless it is clear that previous results are to be re-used, run this always.
    if (! (reuse_SVContainer || m_augment)) {
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

   if (m_PtRescale) { // is a shallow copy
     delete jetShallowAuxContainer;
     delete jetShallowContainer;
   }
   
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
