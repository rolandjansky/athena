
/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/


#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrigLongLivedParticlesHypo/FastTrackFinderLRTHypoTool.h"

using namespace TrigCompositeUtils;

FastTrackFinderLRTHypoTool::FastTrackFinderLRTHypoTool( const std::string& type,const std::string& name,const IInterface* parent )
: AthAlgTool( type, name, parent ),
m_decisionId( HLT::Identifier::fromToolName( name ) ) {}

StatusCode FastTrackFinderLRTHypoTool::initialize()  {
	return StatusCode::SUCCESS;
}

StatusCode FastTrackFinderLRTHypoTool::decide(  TrkCountsInfo& trkinfo )  const {
	if ( trkinfo.previousDecisionIDs.count( m_decisionId.numeric() ) == 0 ) {
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}

	std::vector<int> counts;

	std::vector<float> pTcuts;
	std::vector<float> d0mincuts;
  std::vector<float> d0maxcuts;
  std::vector<float> xi2maxcuts;
	trkinfo.counts->getDetail<std::vector<int>>("counts", counts);
	trkinfo.counts->getDetail<std::vector<float>>("pTcuts", pTcuts);
	trkinfo.counts->getDetail<std::vector<float>>("d0mincuts", d0mincuts);
  trkinfo.counts->getDetail<std::vector<float>>("d0maxcuts", d0maxcuts);
  trkinfo.counts->getDetail<std::vector<float>>("xi2cuts", xi2maxcuts);

	float countForConfiguredPtThreshold{};
	bool found{false};
	ATH_MSG_DEBUG("nCounts: "<<counts.size());
	ATH_MSG_DEBUG("this tool is configured for pT"<< m_min_pt<<" have nPt:"<<pTcuts.size()<<" nd0min: "<<d0mincuts.size()<<" nd0max: "<<d0maxcuts.size()<<" xi2cuts "<<xi2maxcuts.size());

	for ( size_t i = 0; i < counts.size(); ++i ) {
		 if ( std::abs( pTcuts[i] - m_min_pt ) < 0.001 && std::abs( d0mincuts[i] - m_min_d0 ) < 0.001 && std::abs( d0maxcuts[i] - m_max_d0 ) < 0.001 && std::abs( xi2maxcuts[i] - m_max_xi2 ) < 0.001 ) {
			found = true;
			countForConfiguredPtThreshold = counts[i];
		 }
	}
	if (!found ) {
		ATH_MSG_ERROR ("Unable to find tracks count for requested pT threshold " << m_min_pt << " need to fix hypo tool configuratio or add new threshold in tracks counting");
	  for ( float pt : pTcuts ) {
	    ATH_MSG_ERROR( "Count of tracks of pT above " << pt  << " is available");
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
