/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: MuonAODFixAlg.cxx 298140 2013-11-19 11:32:49Z emoyse $

// Gaudi/Athena include(s):
#include "AthenaKernel/errorcheck.h"

// EDM include(s):
#include "xAODMuon/MuonAuxContainer.h"
#include "xAODTruth/TruthParticleContainer.h"

// Local include(s):
#include "MuonAODFixAlg.h"

// Misc
#include "CxxUtils/make_unique.h"
#include <memory>

namespace xAOD {

  MuonAODFixAlg::MuonAODFixAlg( const std::string& name,
    ISvcLocator* svcLoc )
    : AthAlgorithm( name, svcLoc )
  {

    declareProperty( "MuonContainerName", m_containerName = "Muons" );
  }

  StatusCode MuonAODFixAlg::initialize() {
      // Return gracefully:
    return StatusCode::SUCCESS;
    
  }

  StatusCode MuonAODFixAlg::execute() {
    
    // Code copied from InDetVxLinksToMuons
    const SG::DataProxy* proxy =
    evtStore()->proxy (ClassID_traits<xAOD::MuonContainer>::ID(),
                       m_containerName);
    if (!proxy) {
      ATH_MSG_WARNING( "No xAOD::MuonContainer "
                       << "with key \"" << m_containerName << "\" found" );
      return StatusCode::SUCCESS;
    }
        
    xAOD::MuonContainer* muons=nullptr;
    
    if (proxy->isConst()) {
      const xAOD::MuonContainer* originalMuons = nullptr;
      ATH_CHECK( evtStore()->retrieve (originalMuons,
                                       m_containerName) );
    
      muons = new xAOD::MuonContainer;
      auto store = CxxUtils::make_unique<xAOD::MuonAuxContainer>();
      muons->setStore (store.get());
      for (const xAOD::Muon* oldMuon : *originalMuons) {
        xAOD::Muon* muon = new xAOD::Muon;
        muons->push_back (muon);
        *muons->back() = *oldMuon;
        fixMuon(*muon);
      }
      ATH_CHECK( evtStore()->overwrite (muons,
                                    m_containerName,
                                    true, false) );
      ATH_CHECK( evtStore()->overwrite (std::move(store),
                                    m_containerName + "Aux.",
                                    true, false) );
    } else {
      ATH_CHECK( evtStore()->retrieve (muons,
                                       m_containerName) );
       for (auto muon: *muons){
         fixMuon(*muon);
       }
    }
    
    return StatusCode::SUCCESS;
  }

  void MuonAODFixAlg::fixMuon(xAOD::Muon& muon) const {
    // Get primary trackparticle
    const xAOD::TrackParticle * tp = muon.primaryTrackParticle();
    if (!tp) {
       ATH_MSG_WARNING("No primary track particle for this muon. Skipping.");
       return;
    }
    muon.setP4(tp->pt(), tp->eta(), tp->phi());
    if (tp->isAvailable<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" )) {
      ElementLink<xAOD::TruthParticleContainer>& theLink =  muon.auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
      theLink =  tp->auxdata<ElementLink<xAOD::TruthParticleContainer> >("truthParticleLink" );
      muon.auxdata<int>("truthType") = tp->auxdata<int>("truthType");
      muon.auxdata<int>("truthOrigin") = tp->auxdata<int>("truthOrigin");
    }
  }
  
}
