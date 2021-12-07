/*
  Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     TrigJetEJsHypoTool.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
//
// ********************************************************************

#include "TrigJetEJsHypoTool.h"
#include "GaudiKernel/StatusCode.h"

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/xAODJetAsIJetFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/lineSplitter.h"
#include "./DebugInfoCollector.h"
#include "./xAODJetCollector.h"

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "TrigCompositeUtils/TrigCompositeUtils.h"
#include "xAODTrigger/TrigCompositeContainer.h"

using TrigCompositeUtils::DecisionID;
using TrigCompositeUtils::Decision;
using TrigCompositeUtils::DecisionContainer;
using TrigCompositeUtils::DecisionIDContainer;
using TrigCompositeUtils::decisionIDs;

using namespace TrigCompositeUtils;

TrigJetEJsHypoTool::TrigJetEJsHypoTool(const std::string& type,
                                                 const std::string& name,
                                                 const IInterface* parent) :
  AthAlgTool(type, name, parent),
  m_decisionId(HLT::Identifier::fromToolName(name)){

}


TrigJetEJsHypoTool::~TrigJetEJsHypoTool(){
}

StatusCode TrigJetEJsHypoTool::initialize(){

  if (m_visitDebug){

    DebugInfoCollector collector(name());
    CHECK(m_helper->getDescription(collector));
    auto s = collector.toString();
  
    for(const auto& l : lineSplitter(s)){
      ATH_MSG_INFO(l);
    }
  }

  return StatusCode::SUCCESS;
}

StatusCode TrigJetEJsHypoTool::finalize(){
  return StatusCode::SUCCESS;
}

StatusCode
TrigJetEJsHypoTool::decide( std::vector<JetInfo>& input ) const {

  for ( auto i: input ) {

    if ( i.previousDecisionsIDs.count( m_decisionId.numeric() ) == 0 ) {
      continue;
    }

    ATH_MSG_DEBUG("starting exotic/trackless jet chain");

    bool objDecision = true;

    // start with cut on jet pT
    auto jetPt= (i.jet)->pt();
    if((m_trackless == 0 && jetPt < m_jetPtCut_Exotics  ) || \
       (m_trackless == 1 && jetPt < m_jetPtCut_Trackless)) {
      objDecision = false;
    }

    // cut on jet eta
    auto jetEta=std::abs( (i.jet)->p4().Eta() );
    if ( (m_trackless == 0 && jetEta > m_jetEtaCut_Exotics   ) || \
	 (m_trackless == 1 && jetEta > m_jetEtaCut_Trackless ) || \
	 (jetPt <= 0.0)                                      ) {
      objDecision = false;
    }

    float promptTrackFrac = -2.0;

    if (objDecision == true) {
      
      // Exotics (emerging jets) chain decision
      if (m_trackless == 0) {
	promptTrackFrac = 0.0;
	for (auto trackIter = i.AllTracks->begin(); trackIter != i.AllTracks->end(); ++trackIter){
	  float trackPt = (*trackIter)->pt();
	  if (std::abs((*trackIter)->z0() - i.PV->z()) > m_PV_z        || trackPt < m_trackPtCut_Exotics || \
	      std::abs((*trackIter)->eta()) > m_trackEtaCut_Exotics    || (i.jet)->p4().DeltaR((*trackIter)->p4()) > m_dr)  
	    continue;

	  float sigma = 1e6;
	  // Definition of promptTrackFrac background jet d0 RMS 
	  if (trackPt > 0.0) sigma = 1000.0 * 0.0463 / trackPt + 0.0195;
	  
	  // if d0 greater than a multiple of sigma, is not a prompt track
	  if (std::abs((*trackIter)->d0()) > m_PTFSigmaCut*sigma) continue;
	  promptTrackFrac += trackPt;

	  // Exit track loop if PTF is greater than cut, no need to continue loop
	  if (promptTrackFrac/jetPt > m_ptf) break;
	}

	promptTrackFrac /= jetPt;

	ATH_MSG_DEBUG("exotics jets chain promptTrackFrac: " << promptTrackFrac);
	if (promptTrackFrac < -0.5 || promptTrackFrac > m_ptf) objDecision = false;
      }
      
      // Trackless chain decision
      else if (m_trackless == 1) {
	int matchedtracks = 0;
	// Loop over tracks, selection ones passing pT, eta, and dR cuts to jet
	for (auto trackIter = i.AllTracks->begin(); trackIter != i.AllTracks->end(); ++trackIter){
	  if ((*trackIter)->pt() < m_trackPtCut_Trackless || std::abs((*trackIter)->eta()) > m_trackEtaCut_Trackless \
	      || (i.jet)->p4().DeltaR((*trackIter)->p4()) > m_dr)
	    continue;
	  matchedtracks++;
	  if (matchedtracks > m_matchedTracksCut_Trackless) {
	    objDecision = false;
	    break;
	  }

	}
	ATH_MSG_DEBUG("Number of matched tracks (" << matchedtracks << ") greater than cutoff - in trackless jet chain");
      }
      
    } // end if objDecision == true

    if ( objDecision == true ) {
      ATH_MSG_DEBUG("passed exotics/trackless jet chain decision with PTF: " << promptTrackFrac << "\ttrackless: " << m_trackless << "\tpt: " << jetPt << "\teta: " << jetEta);
      addDecisionID( m_decisionId.numeric(), i.decision  );
    }

  } // end loop over inputs

  return StatusCode::SUCCESS;
}
