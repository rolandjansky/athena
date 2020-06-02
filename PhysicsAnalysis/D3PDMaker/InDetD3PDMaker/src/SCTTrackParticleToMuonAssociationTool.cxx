/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//********************************************************/
// @author <laura.rehnisch@cern.ch>                       /
// @date Nov, 2012                                        /
// @brief setting up an association of trackparticles     /
//        and muons.                                      /
//********************************************************/


#include "SCTTrackParticleToMuonAssociationTool.h"
#include "Particle/TrackParticle.h"
#include "muonEvent/MuonContainer.h"
#include "AthenaKernel/errorcheck.h"

#include <cmath>

using namespace Analysis;

namespace D3PD {
  
  SCTTrackParticleToMuonAssociationTool::SCTTrackParticleToMuonAssociationTool (const std::string& type,
										const std::string& name,
										const IInterface* parent)
    : Base (type, name, parent)
  {
    declareProperty ("MuonContainerSGKey", m_muonContainer = "muonCandidate");//dummy, SGKey defined in SCTntuple.py
  }

 
  StatusCode SCTTrackParticleToMuonAssociationTool::initialize()
  {
    CHECK( Base::initialize() ); 
    return StatusCode::SUCCESS; 
  }
 
 
  StatusCode SCTTrackParticleToMuonAssociationTool::book ()
  {
    CHECK( Base::book() );
    return StatusCode::SUCCESS;
  }


  const Analysis::Muon* SCTTrackParticleToMuonAssociationTool::get (const Rec::TrackParticle& TP)
  {
    const Analysis::Muon *muon = 0; 
      
    /// access to MuonTracks
    const MuonContainer* muonsForCombination=0;
    if (evtStore()->retrieve(muonsForCombination, m_muonContainer).isFailure()){
      REPORT_MESSAGE  (MSG::WARNING) << "No AOD muon container found in TDS";
      return 0;
    }
      
    // check if muons in container
    for (const Analysis::Muon* cmuon : *muonsForCombination) {
      /// the Inner Detector track
      if ( cmuon->hasInDetTrackParticle() &&
           std::abs(TP.pt() - cmuon->inDetTrackParticle()->pt()) < 0.01) { 
        muon = cmuon;
      }
	  
    } //muon container loop
	
    return muon;
  }// get()
  
} // namespace D3PD
