/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#if 0 // underlying tool changed to xAOD
// $Id$
/**
 * @file BoostedTopD3PDMaker/src/MiniIsolationMuonFillerTool.cxx
 * @author Lucy Kogan <lucy.anne.kogan@cern.ch>
 * @date Jul 2012
 * @brief Fill mini isolation variables for electron.
 */

#include "BoostedTopD3PDMaker/MiniIsolationMuonFillerTool.h"
#include "IsolationTool/IIsolationTool.h"
#include "muonEvent/Muon.h"
#include "Particle/TrackParticle.h"
#include "AthenaKernel/errorcheck.h"

namespace D3PD {


  /**
   * @brief Standard Gaudi tool constructor.
   * @param type The name of the tool type.
   * @param name The tool name.
   * @param parent The tool's Gaudi parent.
   */
  MiniIsolationMuonFillerTool::MiniIsolationMuonFillerTool (const std::string& type,
						    const std::string& name,
						    const IInterface* parent)
    : BlockFillerTool<Analysis::Muon> (type, name, parent)
  {
    declareProperty ("KtDist", m_ktdist = 10.);
    
    declareProperty("MuIsolationTool",  m_muonIsolationTool);

    declareProperty("minDR", m_mindR=0.01);
    //NB: MAKE SURE MAXDR > MAXDR_2
    declareProperty("maxDR", m_maxdR=0.4);
    declareProperty("maxDR_2", m_maxdR_2=0.3);

    book().ignore();  // Avoid coverity warnings.
  }


  /**
   * @brief Standard Gaudi initialize method.
   */
  StatusCode MiniIsolationMuonFillerTool::initialize()
  {
    ATH_MSG_DEBUG("Trying to retrieve muon tool");
    if(m_muonIsolationTool.retrieve().isFailure()){
      ATH_MSG_DEBUG("Unable to retrieve "<< m_muonIsolationTool);
      return StatusCode::FAILURE;
    }
    else ATH_MSG_DEBUG("Retrieved tool " << m_muonIsolationTool);    

    return StatusCode::SUCCESS;
  }


  /**
   * @brief Book variables for this block.
   */
  StatusCode MiniIsolationMuonFillerTool::book()
  {
    ATH_MSG_DEBUG("book variables");
    CHECK( addVariable ("max40_ptsum",  m_ptsum,  "Muon Mini Isolation using standard isolation tools with dR max 0.4") ); 
    CHECK( addVariable ("max40_nTrks",   m_nTracks,   "Number of tracks used for muon mini isolation with dR max 0.4") );
    CHECK( addVariable ("max30_ptsum",  m_ptsum_2,  "Muon Mini Isolation using standard isolation tools with dR max 0.3") ); 
    CHECK( addVariable ("max30_nTrks",   m_nTracks_2,   "Number of tracks used for muon mini isolation with dR max 0.3") );
    return StatusCode::SUCCESS;
  }


  /**
   * @brief Fill one block --- type-safe version.
   * @param p The input object.
   *
   * This is called once per object.  The caller
   * is responsible for arranging that all the pointers for booked variables
   * are set appropriately upon entry.
   */
  StatusCode MiniIsolationMuonFillerTool::fill (const Analysis::Muon& muon)
  {
    ATH_MSG_DEBUG("filling");
    *m_ptsum = -999999.;
    *m_nTracks = -999999.;
   
    ATH_MSG_DEBUG("muon pt " << muon.pt() << " eta " << muon.eta() << " phi " << muon.phi());

    if(fabs(muon.pt()) > 0){
      //Calculate variable cone size dR
      //      ATH_MSG_INFO("muon pt " << muon.pt());
      double dR = m_ktdist*1000. / muon.pt();
      if ( dR < m_mindR ) { dR = m_mindR; }
      if( dR > m_maxdR ){ dR = m_maxdR; }
      
      ATH_MSG_DEBUG("dR " << dR);

      //FOR TESTING ONLY
      //dR = 0.4;

      const Rec::TrackParticle * trackParticle = 0;
    
      ATH_MSG_DEBUG("get track particle");

      //If the muon has an ID track use this in tool, if not use muon track
      if ( muon.hasInDetTrackParticle() ) trackParticle = muon.inDetTrackParticle();
      else trackParticle = muon.track();

      if(!trackParticle){ATH_MSG_DEBUG("Did not get track particle");}
      else{ATH_MSG_DEBUG(" track particle pt " << trackParticle->pt());}

      //Mini Isolation
      double isolation = m_muonIsolationTool->trackIsolation(trackParticle, dR);
      double isolation_30 = muon.parameter(MuonParameters::ptcone30);
      double isolation_2 = isolation;

      ATH_MSG_DEBUG("isolation " << isolation);

      ATH_MSG_DEBUG("Get number of tracks");
      //Number of tracks
      int numberOfTracks = m_muonIsolationTool->trackIsolationNumberOfTrack(trackParticle, dR);
      int numberOfTracks_30 = muon.parameter(MuonParameters::nucone30);
      int numberOfTracks_2 = numberOfTracks;

      if(dR > m_maxdR_2){
      	isolation_2 = isolation_30;
	numberOfTracks_2 = numberOfTracks_30;
      }

      (*m_ptsum) = isolation;      
      (*m_nTracks) = numberOfTracks;
      (*m_ptsum_2) = isolation_2;      
      (*m_nTracks_2) = numberOfTracks_2;
    }
    else{ATH_MSG_INFO("Muon pt was 0");}

    return StatusCode::SUCCESS;
  }


} // namespace D3PD
#endif
