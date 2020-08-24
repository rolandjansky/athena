/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

//////////////////////////////////////////////////////////////////////////////
// MuonCombinedTool
//  AlgTool performing MS hit reallocation for a likely spectrometer-indet 
//  match which has given combined fit problems.
//  Extrapolates indet track to MS.
//  Returns a combined track with full track fit.
//
//  (c) ATLAS Combined Muon software
//////////////////////////////////////////////////////////////////////////////

#include "GaudiKernel/ConcurrencyFlags.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedTool.h"

namespace MuonCombined {

  MuonCombinedTool::MuonCombinedTool (const std::string& type, const std::string& name, const IInterface* parent) :
    AthAlgTool(type, name, parent)
  {
    declareInterface<IMuonCombinedTool>(this);
    declareProperty("MuonCombinedTagTools", m_muonCombinedTagTools);
  }

  StatusCode MuonCombinedTool::initialize() {

    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_muonCombinedTagTools.retrieve());

    // debug tree, only for running with 1 thread
    if(m_runMuonCombinedDebugger) {
      if (!Gaudi::Concurrency::ConcurrencyFlags::concurrent() || Gaudi::Concurrency::ConcurrencyFlags::numThreads()==1) {
        ATH_CHECK(m_muonCombDebugger.retrieve());
        m_muonCombDebugger->bookBranches();
      }
    }

    return StatusCode::SUCCESS;
  }

  void MuonCombinedTool::combine( const MuonCandidateCollection& muonCandidates, const InDetCandidateCollection& inDetCandidates, std::vector<InDetCandidateToTagMap*> tagMaps, 
				  TrackCollection* combinedTracks, TrackCollection* METracks) const {
   
      // check that there are tracks in both systems
    if( inDetCandidates.empty() ) return;
    if( muonCandidates.empty() )  return;
    if(tagMaps.size()!=m_muonCombinedTagTools.size()){
      ATH_MSG_ERROR("Number of tag maps does not match number of tag tools");
      return;
    }

    // debug tree
    if(m_runMuonCombinedDebugger && !Gaudi::Concurrency::ConcurrencyFlags::concurrent()) {
      m_muonCombDebugger->fillBranches(muonCandidates, inDetCandidates);
    }

    // loop over muon track particles
    for( auto muonCandidate : muonCandidates ){
      const Trk::Track& muonTrack = muonCandidate->extrapolatedTrack() ? *muonCandidate->extrapolatedTrack() : muonCandidate->muonSpectrometerTrack();
      ATH_MSG_DEBUG("MuonCandidate " << m_printer->print(muonTrack) << std::endl << m_printer->printStations(muonTrack) ); 
      // preselect ID candidates close to the muon
      std::vector<const InDetCandidate*> associatedIdCandidates;
      associate(*muonCandidate,inDetCandidates,associatedIdCandidates);
      if( associatedIdCandidates.empty() ) continue;
      ATH_MSG_DEBUG("Associated ID candidates " << associatedIdCandidates.size() );
      // build combined muons
      int count=0;
      for(auto& tool : m_muonCombinedTagTools){
	tool->combine(*muonCandidate,associatedIdCandidates,*(tagMaps.at(count)),combinedTracks,METracks);
	count++;
      }
    }    
  }

  void MuonCombinedTool::associate( const MuonCandidate& muonCandidate, const InDetCandidateCollection& inDetCandidates,  
				    std::vector<const InDetCandidate*>& associatedIdCandidates ) const {

    // define muon (eta,phi)
    double muonEta = 0.;
    double muonPhi = 0.;
    double muonPt = 0.;
    bool hasExtr = muonCandidate.extrapolatedTrack();
    const Trk::TrackParameters* muonPars = hasExtr ? 
      muonCandidate.extrapolatedTrack()->perigeeParameters() : muonCandidate.muonSpectrometerTrack().perigeeParameters();
    if( !muonPars ){
      ATH_MSG_WARNING("MuonCandidate without Perigee, skipping");
      return;
    }
    muonEta = hasExtr ? muonPars->momentum().eta() : muonPars->position().eta();
    muonPhi = hasExtr ? muonPars->momentum().phi() : muonPars->position().phi();
    muonPt  = muonPars->momentum().perp();

    associatedIdCandidates.clear();
    for( auto x : inDetCandidates ){
      double indetEta                         = x->indetTrackParticle().eta();
      double indetPt                          = x->indetTrackParticle().pt();
      double deltaEta                         = fabs(muonEta - indetEta);
      double deltaPhi                         = muonPhi - x->indetTrackParticle().phi();
      double ptBal                            = 1;
      if(muonPt>0) ptBal                      = (muonPt - indetPt)/muonPt;
      if (deltaPhi    > M_PI)  deltaPhi = deltaPhi - 2.*M_PI;
      if (deltaPhi    < -M_PI) deltaPhi = deltaPhi + 2.*M_PI;
      if (deltaEta            > m_deltaEtaPreSelection)  continue;
      if (fabs(deltaPhi)      > m_deltaPhiPreSelection)  continue;
      if (ptBal               > m_ptBalance) continue;
      associatedIdCandidates.push_back(x);
    }
  }

}	// end of namespace
