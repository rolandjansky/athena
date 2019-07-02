
/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/


#include "DecisionHandling/HLTIdentifier.h"
#include "AthenaMonitoring/Monitored.h"

#include "TrackCountHypoTool.h"

using namespace TrigCompositeUtils;

TrackCountHypoTool::TrackCountHypoTool( const std::string& type,const std::string& name,const Interface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode TrackCountHypoTool::initialize()  {
	return StatusCode::SUCCESS;
}

TrackCountHypoTool::~TrackCountHypoTool() {}

StatusCode TrackCountHypoTool::decide(  TrkCountsInfo& trkinfo )  const {
	if ( trkinfo.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}

	std::vector<int> counts;
	std::vector<float> pTcuts;
	trkinfo.counts->getDetail<std::vector<int>>("counts", counts);
	trkinfo.counts->getDetail<std::vector<float>>("pTcuts", pTcuts);
	float countForConfiguredPtThreshold{};
	bool found{false};
	for ( size_t i = 0; i < counts.size(); ++i ) {
		if ( std::abs( pTcuts[i] - m_min_pt ) < 0.001 ) {
			found = true;
			countForConfiguredPtThreshold = counts[i];

		}
	}
	if (!found ) {
		ATH_MSG_DEBUG ("Unable to catch pT thresholds");
		return StatusCode::FAILURE;
	}
	else{
		ATH_MSG_DEBUG ("caught"<<countForConfiguredPtThreshold <<"tracks for "<<m_min_pt<<"");
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
		ATH_MSG_DEBUG ("EVENT ACCOUNTED SUCCESSFULLY!!");
	}
	return StatusCode::SUCCESS;
}
