
/*
Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrackCountHypoTool.h"

using namespace TrigCompositeUtils;

TrackCountHypoTool::TrackCountHypoTool( const std::string& type,const std::string& name,const IInterface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrackCountHypoTool::initialize()  {
	return StatusCode::SUCCESS;
}

StatusCode TrackCountHypoTool::decide(  TrkCountsInfo& trkinfo )  const {
	if ( trkinfo.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}

	std::vector<int> counts;

	std::vector<float> pTcuts;
	std::vector<float> z0cuts;
	trkinfo.counts->getDetail<std::vector<int>>("counts", counts);
	trkinfo.counts->getDetail<std::vector<float>>("pTcuts", pTcuts);
	trkinfo.counts->getDetail<std::vector<float>>("z0cuts", z0cuts);
	float countForConfiguredPtThreshold{};
	bool found{false};
	for ( size_t i = 0; i < counts.size(); ++i ) {
		 if ( std::abs( pTcuts[i] - m_min_pt ) < 0.001 && std::abs( z0cuts[i] - m_max_z0 ) < 0.001 ) {
			found = true;
			countForConfiguredPtThreshold = counts[i];
		 }
	}
	if (!found ) {
		ATH_MSG_ERROR ("Unable to find tracks count for requested pT threshold " << m_min_pt << " need to fix hypo tool configuratio or add new threshold in tracks counting");
	  for ( size_t i = 0; i < counts.size(); ++i ) {
	    ATH_MSG_ERROR( "Count of tracks of pT above " << pTcuts[i]  << " is available");
	  }
	  return StatusCode::FAILURE;
	}
	else{
		ATH_MSG_DEBUG ("REGTEST found "<<countForConfiguredPtThreshold <<" tracks for "<<m_min_pt);
	}
	std::vector<bool> decisionCuts({
		(m_required_ntrks != -1 ? m_required_ntrks <= countForConfiguredPtThreshold  :  (bool)m_logicAnd),
		(m_max_required_ntrks != -1 ? m_max_required_ntrks > countForConfiguredPtThreshold : (bool)m_logicAnd)
	});

	if(m_logicAnd && !std::all_of(decisionCuts.begin(),decisionCuts.end(),[](bool k ){return k; }) ){

		return StatusCode::SUCCESS;
	}else if(m_logicAnd==false && !std::any_of(decisionCuts.begin(),decisionCuts.end(),[](bool k ){return k; }) ){
		return StatusCode::SUCCESS;
	}else{
		addDecisionID( m_decisionId.numeric(), trkinfo.decision );
		ATH_MSG_DEBUG ("REGTEST event accepted");
	}
	return StatusCode::SUCCESS;
}
