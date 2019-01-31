/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

/***************************************************************************
                           BTagTool.cxx  -  Description
                             -------------------
    begin   : 10.03.04
    authors : Andreas Wildauer (CERN PH-ATC), Fredrik Akesson (CERN PH-ATC)
    email   : andreas.wildauer@cern.ch, fredrik.akesson@cern.ch

    changes : 13.01.05 (FD) introduce soft lepton case

***************************************************************************/

#include "BTagging/BTagTool.h"

#include "xAODTracking/Vertex.h"
#include "xAODTracking/VertexContainer.h"
#include "VxVertex/VxTrackAtVertex.h"

#include "BTagging/BTagLabeling.h"
#include "JetTagTools/ITagTool.h"

namespace Analysis {

  BTagTool::BTagTool(const std::string& t, const std::string& n, const IInterface* p) :
    AthAlgTool(t,n,p),
    m_bTagToolHandleArray(this),
    m_bTagTool(std::map<std::string, ITagTool*>()),
    m_BaselineTagger("IP3D+SV1"),
    m_vxPrimaryName("PrimaryVertices"),
    m_runModus("analysis"),
    m_BTagLabelingTool("Analysis::BTagLabeling", this)
  {
    declareInterface<IBTagTool>(this);

    // Name of StoreGate collections
    declareProperty("PrimaryVertexName",  m_vxPrimaryName);

    // List of the tagging tools to be used, HAS TO BE given by jobOptions
    declareProperty("TagToolList",                    m_bTagToolHandleArray);

    declareProperty("BaselineTagger",                 m_BaselineTagger);

    declareProperty("Runmodus"   ,                    m_runModus); // The run modus (reference/analysis)
    declareProperty("BTagLabelingTool",               m_BTagLabelingTool);
  }

  BTagTool::~BTagTool() {}

  StatusCode BTagTool::initialize() {
    
    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_VertexCollectionName.initialize() );
   
    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */
    if ( m_bTagToolHandleArray.empty() ) {
      ATH_MSG_DEBUG("#BTAG# No tagging tools defined. Please revisit configuration.");
    }
    else {
      if ( m_bTagToolHandleArray.retrieve().isFailure() ) {
        ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_bTagToolHandleArray);
        return StatusCode::FAILURE;
      } else {
        ATH_MSG_DEBUG("#BTAG# Retrieved " << m_bTagToolHandleArray);
      }
    }

    // get BJetTagTools
    ToolHandleArray< ITagTool >::iterator itTagTools = m_bTagToolHandleArray.begin();
    ToolHandleArray< ITagTool >::iterator itTagToolsEnd = m_bTagToolHandleArray.end();
    for (  ; itTagTools != itTagToolsEnd; ++itTagTools ) {
      ATH_MSG_DEBUG("#BTAG# "<< name() << " inserting: " << (*itTagTools).typeAndName() << " to tools list.");
      m_bTagTool.insert(std::pair<std::string, ITagTool*>((*itTagTools).name(), &(*(*itTagTools)))); 
    }

    // retrieve the truth labeling tool
    if ( !m_BTagLabelingTool.empty() ) {
      if ( m_BTagLabelingTool.retrieve().isFailure() ) {
	ATH_MSG_FATAL("#BTAG# Failed to retrieve tool " << m_BTagLabelingTool);
	return StatusCode::FAILURE;
      } else {
	ATH_MSG_DEBUG("#BTAG# Retrieved tool " << m_BTagLabelingTool);
      }
    }
    
    // JBdV for debugging
    m_nBeamSpotPvx = 0;
    m_nAllJets     = 0;

    return StatusCode::SUCCESS;
  }


  StatusCode BTagTool::tagJet(xAOD::Jet& jetToTag, xAOD::BTagging* BTag, const xAOD::Vertex* vtx) {

    ATH_MSG_VERBOSE("#BTAG# (p, E) of original Jet: (" << jetToTag.px() << ", " << jetToTag.py() << ", "
		    << jetToTag.pz() << "; " << jetToTag.e() << ") MeV");
    m_nAllJets++;

    /* ----------------------------------------------------------------------------------- */
    /*               Truth Labeling                  				           */
    /* ----------------------------------------------------------------------------------- */

    StatusCode jetIsLabeled( StatusCode::SUCCESS );
    if (!m_BTagLabelingTool.empty()) {
      jetIsLabeled = m_BTagLabelingTool->BTagLabeling_exec( jetToTag);
    }
    if ( jetIsLabeled.isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to do truth labeling");
      return StatusCode::FAILURE;
    }

    /* ----------------------------------------------------------------------------------- */
    /*               RETRIEVE PRIMARY VERTEX CONTAINER FROM STOREGATE                      */
    /* ----------------------------------------------------------------------------------- */
    const xAOD::Vertex* primaryVertex(0);
    StatusCode sc = StatusCode::SUCCESS;

    if (vtx) {
      primaryVertex = vtx;
    } else {
      const xAOD::VertexContainer* vxContainer(0);
      
      sc = evtStore()->retrieve(vxContainer, m_vxPrimaryName);
      
      if (sc.isFailure()) {
	ATH_MSG_WARNING("#BTAG# Primary vertex coll " << m_vxPrimaryName << " not found");
	return StatusCode::SUCCESS;
      }
    
      if (vxContainer->size()==0) {
	ATH_MSG_DEBUG("#BTAG#  Vertex container is empty");
	return StatusCode::SUCCESS;
      }

      for (xAOD::VertexContainer::const_iterator fz = vxContainer->begin(); fz != vxContainer->end(); ++fz) {
	if ((*fz)->vertexType() == xAOD::VxType::PriVtx) {
	  primaryVertex = *fz;
	  break;
	}
      }
      
      if (! primaryVertex) {
	ATH_MSG_DEBUG("#BTAG#  No vertex labeled as VxType::PriVtx!");
	xAOD::VertexContainer::const_iterator fz = vxContainer->begin();
        primaryVertex = *fz;
	if (primaryVertex->nTrackParticles() == 0) {
	  ATH_MSG_DEBUG("#BTAG#  PV==BeamSpot: probably poor tagging");
	  m_nBeamSpotPvx++;
        }
      }
    }

    /* ----------------------------------------------------------------------------------- */
    /*               Call all the tag tools specified in m_bTagToolHandleArray             */
    /* ----------------------------------------------------------------------------------- */

    ToolHandleArray< ITagTool >::iterator itTagTools = m_bTagToolHandleArray.begin();
    ToolHandleArray< ITagTool >::iterator itTagToolsEnd = m_bTagToolHandleArray.end();
    for (  ; itTagTools != itTagToolsEnd; ++itTagTools ) {
      (*itTagTools)->setOrigin(primaryVertex);
      sc = (*itTagTools)->tagJet(jetToTag, BTag);
      if (sc.isFailure()) {
        ATH_MSG_WARNING("#BTAG# failed tagger: " << (*itTagTools).typeAndName() );
      }

    }
/*    if (m_runModus == "analysis") {
      double weight = -100.;
      if (m_BaselineTagger == "IP3D+SV1")
	weight = jetToTag.getFlavourTagWeight();
      else 
	weight = jetToTag.getFlavourTagWeight(m_BaselineTagger);
      std::vector<double> w; w.reserve(1); w.push_back(weight);
      jetToTag.setCombinedLikelihood(w);
    }
*/

    // ----------------------------------------------------------------------------------

    ATH_MSG_VERBOSE("#BTAG#  tagJet(...) end.");
    return StatusCode::SUCCESS;
  }

  StatusCode BTagTool::tagJet(const xAOD::JetContainer * jetContainer, xAOD::BTaggingContainer * btaggingContainer) {

    /* ----------------------------------------------------------------------------------- */
    /*               RETRIEVE PRIMARY VERTEX CONTAINER FROM STOREGATE                      */
    /* ----------------------------------------------------------------------------------- */
    SG::ReadHandle<xAOD::VertexContainer> h_VertexCollectionName (m_VertexCollectionName);
    if (!h_VertexCollectionName.isValid()) {
        ATH_MSG_ERROR( " cannot retrieve primary vertex container with key " << m_VertexCollectionName.key()  );
        return StatusCode::FAILURE;
    }
    unsigned int nVertexes = h_VertexCollectionName->size();
    if (nVertexes == 0) {
      ATH_MSG_DEBUG("#BTAG#  Vertex container is empty");
      return StatusCode::SUCCESS;
    }

    const xAOD::Vertex* primaryVertex(0);
    for (xAOD::VertexContainer::const_iterator fz = h_VertexCollectionName->begin(); fz != h_VertexCollectionName->end(); ++fz) {
      if ((*fz)->vertexType() == xAOD::VxType::PriVtx) {
	primaryVertex = *fz;
	break;
      }
    }


    if (! primaryVertex) {
      ATH_MSG_DEBUG("#BTAG#  No vertex labeled as VxType::PriVtx!");
      xAOD::VertexContainer::const_iterator fz = h_VertexCollectionName->begin();
      primaryVertex = *fz;
        if (primaryVertex->nTrackParticles() == 0) {
	  ATH_MSG_DEBUG("#BTAG#  PV==BeamSpot: probably poor tagging");
          m_nBeamSpotPvx++;
      }
    }

    xAOD::BTaggingContainer::iterator btagIter=btaggingContainer->begin();
    for (xAOD::JetContainer::const_iterator jetIter = jetContainer->begin(); jetIter != jetContainer->end(); ++jetIter, ++btagIter) {
      //temporary const_cast
      xAOD::Jet& jetToTag = const_cast<xAOD::Jet&>( **jetIter );
      ATH_MSG_VERBOSE("#BTAG# (p, E) of original Jet: (" << jetToTag.px() << ", " << jetToTag.py() << ", "
		    << jetToTag.pz() << "; " << jetToTag.e() << ") MeV");
      m_nAllJets++;

      /* ----------------------------------------------------------------------------------- */
      /*               Truth Labeling                  				           */
      /* ----------------------------------------------------------------------------------- */

      StatusCode jetIsLabeled( StatusCode::SUCCESS );
      if (!m_BTagLabelingTool.empty()) {
        jetIsLabeled = m_BTagLabelingTool->BTagLabeling_exec( jetToTag);
      }
      if ( jetIsLabeled.isFailure() ) {
        ATH_MSG_ERROR("#BTAG# Failed to do truth labeling");
        continue;
        //return StatusCode::FAILURE;
      } 

      /* ----------------------------------------------------------------------------------- */
      /*               Call all the tag tools specified in m_bTagToolHandleArray             */
      /* ----------------------------------------------------------------------------------- */

      ToolHandleArray< ITagTool >::iterator itTagTools = m_bTagToolHandleArray.begin();
      ToolHandleArray< ITagTool >::iterator itTagToolsEnd = m_bTagToolHandleArray.end();
      for (  ; itTagTools != itTagToolsEnd; ++itTagTools ) {
        (*itTagTools)->setOrigin(primaryVertex);
        StatusCode sc = (*itTagTools)->tagJet(jetToTag, *btagIter);
        if (sc.isFailure()) {
          ATH_MSG_WARNING("#BTAG# failed tagger: " << (*itTagTools).typeAndName() );
        }

      }

      // ----------------------------------------------------------------------------------

      ATH_MSG_VERBOSE("#BTAG#  tagJet(...) end.");
    }

    return StatusCode::SUCCESS;
  }
  
  StatusCode BTagTool::finalize() {
    ATH_MSG_INFO("#BTAG# number of jets with Primary Vertex set to BeamSpot " << m_nBeamSpotPvx << " Total number of jets seen " << m_nAllJets);
    return StatusCode::SUCCESS;
  }


  /** This method can be called in the finalize() method of the caller algorithm */
  void BTagTool::finalizeHistos() {
  //   ITagTool * myTagTool(0);
  //   /// finalize calculates the integratedIP histos
  //   myTagTool = m_bTagTool["LifetimeTag1D"];
  //   if (myTagTool!=0)  myTagTool->finalizeHistos();
  //   myTagTool = m_bTagTool["LifetimeTag2D"];
  //   if (myTagTool!=0)  myTagTool->finalizeHistos();
  //   myTagTool = m_bTagTool["SecVtxTagBU"];
  //   if (myTagTool!=0)  myTagTool->finalizeHistos();
  }

}

