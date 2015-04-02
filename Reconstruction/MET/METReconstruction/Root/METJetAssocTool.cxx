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
    const xAOD::CaloClusterContainer* tcCont;
    const xAOD::Vertex* pv;
    const xAOD::TrackParticleContainer* trkCont;
    const xAOD::PFOContainer* pfoCont;
    if (retrieveConstituents(tcCont,pv,trkCont,pfoCont).isFailure()) {
      ATH_MSG_WARNING("Unable to retrieve constituent containers");
      return StatusCode::FAILURE;
    }

    // Create jet associations
    MissingETBase::Types::constvec_t trkvec;
    for(const auto& jet : *jetCont) {
      std::vector<const IParticle*> selectedTracks;
      if (m_pflow) {
        for (size_t consti = 0; consti < jet->numConstituents(); consti++) {
          const xAOD::PFO *pfo = static_cast<const xAOD::PFO*>(jet->rawConstituent(consti));
          if (pfo->charge()!=0) {
	    trkvec += *pfo;
	    selectedTracks.push_back(pfo->track(0));
	  }
        }
      } else {
	if(pv) {
	  std::vector<const IParticle*> jettracks;
	  jet->getAssociatedObjects<IParticle>(JetAttribute::GhostTrack,jettracks);

	  selectedTracks.reserve(jettracks.size());
	  for(const auto& trk : jettracks) {
	    const TrackParticle* pTrk = static_cast<const TrackParticle*>(trk);
	    if(acceptTrack(pTrk,pv) && isGoodEoverP(pTrk,tcCont,pv,trkCont)) {
	      selectedTracks.push_back(trk);
	      ATH_MSG_VERBOSE("Accept track " << trk << " px, py = " << trk->p4().Px() << ", " << trk->p4().Py());
	    }
	  }
	}
      }
      MissingETComposition::add(metMap,jet,selectedTracks);
      //if (m_pflow) metMap->back()->setJetTrkVec(trkvec);
      ATH_MSG_VERBOSE("Added association " << metMap->findIndex(jet) << " pointing to jet " << jet);
      ATH_MSG_VERBOSE("Jet pt, eta, phi = " << jet->pt() << ", " << jet->eta() << "," << jet->phi() );

    }
    MissingETComposition::addMiscAssociation(metMap);
    ATH_MSG_DEBUG("Added miscellaneous association");

    return StatusCode::SUCCESS;
  }

}
