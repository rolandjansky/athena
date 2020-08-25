/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#include "BTagging/JetSecVtxFindingAlg.h"

//general interface for secondary vertex finders
#include "InDetRecToolInterfaces/ISecVertexInJetFinder.h"
#include "VxSecVertex/VxSecVKalVertexInfo.h"
#include "VxSecVertex/VxJetFitterVertexInfo.h"
 
#include "StoreGate/ReadDecorHandle.h"
#include "StoreGate/ReadHandle.h"

namespace Analysis {

  JetSecVtxFindingAlg::JetSecVtxFindingAlg(const std::string& name, ISvcLocator* pSvcLocator):
    AthAlgorithm(name,pSvcLocator),
    m_secVertexFinderToolHandle(this),
    m_vxPrimaryName("PrimaryVertices")
  {
    declareProperty("PrimaryVertexName",  m_vxPrimaryName);
    //List of the secondary vertex finders in jet to be run
    declareProperty("SecVtxFinder",          m_secVertexFinderToolHandle);
  }

  JetSecVtxFindingAlg::~JetSecVtxFindingAlg() {
  }

  StatusCode JetSecVtxFindingAlg::initialize()
  {
    // This will check that the properties were initialized properly
    // by job configuration.
    ATH_CHECK( m_JetCollectionName.initialize() );
    ATH_CHECK( m_jetParticleLinkName.initialize() );
    ATH_CHECK( m_VertexCollectionName.initialize() );
    ATH_CHECK( m_VxSecVertexInfoName.initialize() );

    /* ----------------------------------------------------------------------------------- */
    /*                        RETRIEVE SERVICES FROM STOREGATE                             */
    /* ----------------------------------------------------------------------------------- */

    if ( m_secVertexFinderToolHandle.retrieve().isFailure() ) {
      ATH_MSG_ERROR("#BTAG# Failed to retrieve " << m_secVertexFinderToolHandle);
    } else {
      ATH_MSG_DEBUG("#BTAG# Retrieved " << m_secVertexFinderToolHandle);
    }

    return StatusCode::SUCCESS;
  }


  StatusCode JetSecVtxFindingAlg::execute() {
    //retrieve the Jet container
    SG::ReadHandle<xAOD::JetContainer> h_JetCollectionName (m_JetCollectionName);
    if (!h_JetCollectionName.isValid()) {
      ATH_MSG_ERROR( " cannot retrieve jet container with key " << m_JetCollectionName.key()  );
      return StatusCode::FAILURE;
    }

    /* Record the VxSecVertexInfo output container */
    SG::WriteHandle<Trk::VxSecVertexInfoContainer> h_VxSecVertexInfoName (m_VxSecVertexInfoName);
    ATH_CHECK( h_VxSecVertexInfoName.record(std::make_unique<Trk::VxSecVertexInfoContainer>()));

    if (h_JetCollectionName->size() == 0) {
      ATH_MSG_DEBUG("#BTAG# Empty Jet collection");
      return StatusCode::SUCCESS;
    }

    SG::ReadDecorHandle<xAOD::JetContainer, std::vector<ElementLink< xAOD::TrackParticleContainer> > > h_jetParticleLinkName (m_jetParticleLinkName);
    if (!h_jetParticleLinkName.isAvailable()) {
      ATH_MSG_ERROR( " cannot retrieve jet container particle EL decoration with key " << m_jetParticleLinkName.key()  );
      return StatusCode::FAILURE;
    }
 
    const xAOD::Vertex* primaryVertex(0);

    //retrieve primary vertex
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
        }
    }

    const xAOD::Vertex& PrimaryVtx = *primaryVertex;

    for (xAOD::JetContainer::const_iterator jetIter = h_JetCollectionName->begin(); jetIter != h_JetCollectionName->end(); ++jetIter) {
      const xAOD::Jet& jetToTag = **jetIter;

      //Get it from decor jet
      const std::vector<ElementLink< xAOD::TrackParticleContainer > >& tracksInJet = h_jetParticleLinkName(jetToTag);

      if(tracksInJet.size()==0){
        ATH_MSG_DEBUG("#BTAG# No track in Jet");
        h_VxSecVertexInfoName->push_back(nullptr);
        continue;
      } 
   
      std::vector<const xAOD::IParticle*> inputIParticles;

      std::vector<ElementLink< xAOD::TrackParticleContainer > >::const_iterator itEL = tracksInJet.begin();
      std::vector<ElementLink< xAOD::TrackParticleContainer > >::const_iterator itELend = tracksInJet.end();
       
      //Before splitting empty tracksInJet case

      for (  ; itEL != itELend; ++itEL ) {
        const xAOD::TrackParticle* const *inputTrackParticle ;
        inputTrackParticle = (*itEL).cptr(); //ElementConstPointer cptr

	      /// warning -> will not work if at some point we decide to associate to several track collections at the same time (in the same assoc object)
	  
        inputIParticles.push_back(*inputTrackParticle);
      }

      ATH_MSG_DEBUG("#BTAG#  Running " << m_secVertexFinderToolHandle);

      Trk::VxSecVertexInfo* myVertexInfo = m_secVertexFinderToolHandle->findSecVertex(PrimaryVtx, (*jetIter)->p4(), inputIParticles);
      ATH_MSG_DEBUG("#BTAG# Number of vertices found: " << myVertexInfo->vertices().size());
      h_VxSecVertexInfoName->push_back(myVertexInfo); 
    }// for loop on jets

    return StatusCode::SUCCESS;
  } 

} // namespace
