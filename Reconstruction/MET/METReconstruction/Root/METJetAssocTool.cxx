///////////////////////// -*- C++ -*- /////////////////////////////

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// METJetAssocTool.cxx 
// Implementation file for class METJetAssocTool
//
//  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
//
// Author: P Loch, S Resconi, TJ Khoo, AS Mete
/////////////////////////////////////////////////////////////////// 

// METReconstruction includes
#include "METReconstruction/METJetAssocTool.h"

// MET EDM
#include "xAODMissingET/MissingETComposition.h"

// Jet EDM
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAttributes.h"

// Tracking EDM
#include "xAODTracking/Vertex.h"

namespace met {

  using namespace xAOD;

  // Constructors
  ////////////////
  METJetAssocTool::METJetAssocTool(const std::string& name) : 
    AsgTool(name),
    METAssociator(name)
  {}

  // Destructor
  ///////////////
  METJetAssocTool::~METJetAssocTool()
  {}

  // Athena algtool's Hooks
  ////////////////////////////
  StatusCode METJetAssocTool::initialize()
  {
    ATH_MSG_VERBOSE ("Initializing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  StatusCode METJetAssocTool::finalize()
  {
    ATH_MSG_VERBOSE ("Finalizing " << name() << "...");
    return StatusCode::SUCCESS;
  }

  /////////////////////////////////////////////////////////////////// 
  // Const methods: 
  ///////////////////////////////////////////////////////////////////

  /////////////////////////////////////////////////////////////////// 
  // Non-const methods: 
  /////////////////////////////////////////////////////////////////// 

  /////////////////////////////////////////////////////////////////// 
  // Protected methods: 
  /////////////////////////////////////////////////////////////////// 

  // executeTool
  ////////////////
  StatusCode METJetAssocTool::executeTool(xAOD::MissingETContainer* /*metCont*/, xAOD::MissingETAssociationMap* metMap) 
  {
    ATH_MSG_VERBOSE ("In execute: " << name() << "...");

    // Retrieve the jet container
    const JetContainer* jetCont = 0;
    if( evtStore()->retrieve(jetCont, m_input_data_key).isFailure() ) {
      ATH_MSG_WARNING("Unable to retrieve input jet container " << m_input_data_key);
      return StatusCode::FAILURE;
    }
    ATH_MSG_DEBUG("Successfully retrieved jet collection");

    // Retrieve the vertices
    const VertexContainer* vxCont = 0;
    if( evtStore()->retrieve(vxCont,"PrimaryVertices").isFailure() ) { // configurable
      ATH_MSG_WARNING("Unable to retrieve vertex container");
      return StatusCode::FAILURE;
    } 
    const Vertex* pv = 0;
    ATH_MSG_DEBUG("Successfully retrieved vertex collection");
    for(const auto& vx : *vxCont) {
      if(vx->vertexType()==VxType::PriVtx)
	{pv = vx; break;}
    }
    if(!pv) {
      ATH_MSG_WARNING("Failed to find primary vertex!");
      return StatusCode::FAILURE;
    }

    // Create jet associations
    MissingETBase::Types::constvec_t trkvec;
    for(const auto& jet : *jetCont) {
      std::vector<const IParticle*> selectedTracks;
      if (m_pflow) {
        for (size_t consti = 0; consti < jet->numConstituents(); consti++) {
          const xAOD::PFO *pfo = dynamic_cast<const xAOD::PFO*>(jet->rawConstituent(consti));
          if (pfo->charge()!=0) trkvec += *pfo;
        }
      } else {
        std::vector<const IParticle*> jettracks;
        jet->getAssociatedObjects<IParticle>(JetAttribute::GhostTrack,jettracks);

	selectedTracks.reserve(jettracks.size());
	for(const auto& trk : jettracks) {
	  const TrackParticle* pTrk = dynamic_cast<const TrackParticle*>(trk);
	  if( acceptTrack(pTrk,pv) ) {
	    selectedTracks.push_back(trk);
	    ATH_MSG_VERBOSE("Accept track " << trk << " px, py = " << trk->p4().Px() << ", " << trk->p4().Py());
	  }
	}
      }
      MissingETComposition::add(metMap,jet,selectedTracks);
      if (m_pflow) metMap->back()->setJetTrkVec(trkvec);
      ATH_MSG_VERBOSE("Added association " << metMap->findIndex(jet) << " pointing to jet " << jet);
      ATH_MSG_VERBOSE("Jet pt, eta, phi = " << jet->pt() << ", " << jet->eta() << "," << jet->phi() );

    }
    MissingETComposition::addMiscAssociation(metMap);
    ATH_MSG_DEBUG("Added miscellaneous association");

    return StatusCode::SUCCESS;
  }

}
