/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/*
 * TrackIsolationDecorAlg.cxx
 *
 *  Created on: Sep 18, 2014
 *      Author: goblirsc
 */


#include "TrackIsolationDecorAlg.h"
#include "GaudiKernel/Property.h"
#include "xAODMuon/MuonContainer.h"
#include "xAODTracking/TrackParticleContainer.h"
#include "GaudiKernel/ITHistSvc.h"

using std::string;

//**********************************************************************

TrackIsolationDecorAlg::TrackIsolationDecorAlg(const std::string& name, ISvcLocator* pSvcLocator ) :
  AthAlgorithm( name, pSvcLocator ),
  m_histSvc("THistSvc", name)
# if !defined(XAOD_ANALYSIS)
  ,m_track_iso_tool()
# endif
{
# if !defined(XAOD_ANALYSIS)
  declareProperty("TrackIsolationTool",        m_track_iso_tool);
  declareProperty("TrackCollection",        m_track_collection="InDetTrackParticles");
  declareProperty("PtMin",        m_pt_min=10.);
# endif
  
  m_trk_iso_types = {xAOD::Iso::ptcone40, xAOD::Iso::ptcone30, xAOD::Iso::ptcone20  };
                                                
  m_trk_corr.trackbitset = xAOD::Iso::coreTrackPtr;
  
}

//**********************************************************************

TrackIsolationDecorAlg::~TrackIsolationDecorAlg() { }

//**********************************************************************

StatusCode TrackIsolationDecorAlg::initialize() {

  ATH_CHECK(m_histSvc.retrieve());
# if !defined(XAOD_ANALYSIS)
  ATH_CHECK(m_track_iso_tool.retrieve());
# endif

  return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrackIsolationDecorAlg::finalize() {

	return StatusCode::SUCCESS;
}

//**********************************************************************

StatusCode TrackIsolationDecorAlg::execute() {
# if !defined(XAOD_ANALYSIS)
  const xAOD::TrackParticleContainer* tracks = 0;
  if (evtStore()->retrieve(tracks,m_track_collection).isFailure()){
    ATH_MSG_WARNING( "Unable to retrieve Track Container" );
  }
  
  static SG::AuxElement::Decorator< float > cone20dec( "ptcone20" );
  static SG::AuxElement::Decorator< float > cone30dec( "ptcone30" );
  static SG::AuxElement::Decorator< float > cone40dec( "ptcone40" );
  static SG::AuxElement::Decorator< float > varcone20dec( "ptvarcone20" );
  static SG::AuxElement::Decorator< float > varcone30dec( "ptvarcone30" );
  static SG::AuxElement::Decorator< float > varcone40dec( "ptvarcone40" );
  for (auto trk : *tracks){
      xAOD::TrackIsolation result; 
      if (trk->pt() < m_pt_min) continue;
      if (!m_track_iso_tool->trackIsolation(result, *trk, m_trk_iso_types,m_trk_corr,0,0,tracks)){
          ATH_MSG_WARNING("Unable to decorate track isolation!!");
      }
      else{
          cone40dec(*trk) = result.ptcones[0];
          cone30dec(*trk) = result.ptcones[1];
          cone20dec(*trk) = result.ptcones[2];
          varcone40dec(*trk) = result.ptvarcones_10GeVDivPt[0];
          varcone30dec(*trk) = result.ptvarcones_10GeVDivPt[1];
          varcone20dec(*trk) = result.ptvarcones_10GeVDivPt[2];
      }
      
  }
# endif
  return StatusCode::SUCCESS;
}

//**********************************************************************
