/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
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

//<<<<<< INCLUDES                                                       >>>>>>
#include "MuonRecHelperTools/MuonEDMPrinterTool.h"

#include "MuonCombinedToolInterfaces/IMuonCombinedTagTool.h"
#include "MuonCombinedEvent/InDetCandidate.h"
#include "MuonCombinedEvent/MuonCandidate.h"
#include "MuonCombinedTool.h"

namespace MuonCombined {
 
  //<<<<<< CLASS STRUCTURE INITIALIZATION                                 >>>>>>

  MuonCombinedTool::MuonCombinedTool (const std::string& type, const std::string& name, const IInterface* parent)
    :	AthAlgTool(type, name, parent),
	m_printer("Muon::MuonEDMPrinterTool/MuonEDMPrinterTool")
	//m_muonCombinedTagTools("MuonCombined::MuonCombinedStacoTagTool/MuonCombinedStacoTagTool")
  {
    declareInterface<IMuonCombinedTool>(this);
    declareProperty("Printer",m_printer );
    declareProperty("MuonCombinedTagTools",     m_muonCombinedTagTools);
    declareProperty("DeltaEtaPreSelection",     m_deltaEtaPreSelection = 0.5 );
    declareProperty("DeltaPhiPreSelection",     m_deltaPhiPreSelection = 1.  );
  }

  MuonCombinedTool::~MuonCombinedTool()
  {}

  //<<<<<< PUBLIC MEMBER FUNCTION DEFINITIONS                             >>>>>>

  StatusCode MuonCombinedTool::initialize() {

    ATH_CHECK(m_printer.retrieve());
    ATH_CHECK(m_muonCombinedTagTools.retrieve());

    return StatusCode::SUCCESS;
  }

  StatusCode MuonCombinedTool::finalize() {
    return StatusCode::SUCCESS;
  }

  void MuonCombinedTool::combine( const MuonCandidateCollection& muonCandidates,  const InDetCandidateCollection& inDetCandidates ) const {
   
      // check that there are tracks in both systems
    if( inDetCandidates.empty() ) return;
    if( muonCandidates.empty() )  return;


    // loop over muon track particles
    for( auto muonCandidate : muonCandidates ){
      const Trk::Track& muonTrack = muonCandidate->extrapolatedTrack() ? *muonCandidate->extrapolatedTrack() : muonCandidate->muonSpectrometerTrack();
      ATH_MSG_DEBUG("MuonCandidate " << m_printer->print(muonTrack) << std::endl << m_printer->printStations(muonTrack) ); 
      // preselect ID candidates close to the muon
      std::vector<InDetCandidate*> associatedIdCandidates;
      associate(*muonCandidate,inDetCandidates,associatedIdCandidates);
      if( associatedIdCandidates.empty() ) continue;
      ATH_MSG_DEBUG("Associated ID candidates " << associatedIdCandidates.size() );
      // build combined muons
      for(auto tool : m_muonCombinedTagTools) 
	tool->combine(*muonCandidate,associatedIdCandidates);
      
    }    
  }

  void MuonCombinedTool::associate( const MuonCandidate& muonCandidate, const InDetCandidateCollection& inDetCandidates,  
				    std::vector<InDetCandidate*>& associatedIdCandidates ) const {

    // define muon (eta,phi)
    double muonEta = 0.;
    double muonPhi = 0.;
    bool hasExtr = muonCandidate.extrapolatedTrack();
    const Trk::TrackParameters* muonPars = hasExtr ? 
      muonCandidate.extrapolatedTrack()->perigeeParameters() : muonCandidate.muonSpectrometerTrack().perigeeParameters();
    if( !muonPars ){
      ATH_MSG_WARNING("MuonCandidate without Perigee, skipping");
      return;
    }
    muonEta = hasExtr ? muonPars->momentum().eta() : muonPars->position().eta();
    muonPhi = hasExtr ? muonPars->momentum().phi() : muonPars->position().phi();

    associatedIdCandidates.clear();
    for( auto x : inDetCandidates ){
      double indetEta                         = x->indetTrackParticle().eta();
      double deltaEta                         = fabs(muonEta - indetEta);
      double deltaPhi                         = muonPhi - x->indetTrackParticle().phi();
      if (deltaPhi    > M_PI)  deltaPhi = deltaPhi - 2.*M_PI;
      if (deltaPhi    < -M_PI) deltaPhi = deltaPhi + 2.*M_PI;
      if (deltaEta            > m_deltaEtaPreSelection)  continue;
      if (fabs(deltaPhi)      > m_deltaPhiPreSelection)  continue;
      associatedIdCandidates.push_back(x);
    }
  }

}	// end of namespace
