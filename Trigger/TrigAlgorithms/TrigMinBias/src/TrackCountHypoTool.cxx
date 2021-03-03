
/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "TrackCountHypoTool.h"

using namespace TrigCompositeUtils;

TrackCountHypoTool::TrackCountHypoTool(const std::string &type, const std::string &name, const IInterface *parent)
	: AthAlgTool(type, name, parent),
	  m_decisionId(HLT::Identifier::fromToolName(name)) {}


StatusCode TrackCountHypoTool::decide(TrkCountsInfo &trkinfo) const
{
	if (trkinfo.previousDecisionIDs.count(m_decisionId.numeric()) == 0)
	{
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}
	if ( m_acceptAll ) {		
		addDecisionID(m_decisionId.numeric(), trkinfo.decision);
		ATH_MSG_DEBUG("REGTEST event accepted because of acceptAll");
	}

	std::vector<int> counts;

	std::vector<float> pTcuts;
	std::vector<float> z0cuts;
	trkinfo.counts->getDetail<std::vector<int>>("counts", counts);
	trkinfo.counts->getDetail<std::vector<float>>("pTcuts", pTcuts);
	trkinfo.counts->getDetail<std::vector<float>>("z0cuts", z0cuts);
	float countForConfiguredPtThreshold{};
	bool found{false};
	for (size_t i = 0; i < counts.size(); ++i)
	{
		if (std::abs(pTcuts[i] - m_minPt) < 0.001 && std::abs(z0cuts[i] - m_maxZ0) < 0.001)
		{
			found = true;
			countForConfiguredPtThreshold = counts[i];
		}
	}

	if (!found)
	{
		ATH_MSG_ERROR("Unable to find tracks count for requested pT threshold " << m_minPt << " need to fix hypo tool configuration or add new threshold in tracks counting");
		for (size_t i = 0; i < counts.size(); ++i)
		{
			ATH_MSG_ERROR("Count of tracks of pT above " << pTcuts[i] << " and z0 requirement "  << z0cuts[i] << " that are available");
		}
		return StatusCode::FAILURE;
	}
	else
	{
		ATH_MSG_DEBUG("REGTEST found " << countForConfiguredPtThreshold << " tracks for " << m_minPt);
	}
	
	const bool minTrkPassed = (m_minNtrks == -1) or (countForConfiguredPtThreshold > m_minNtrks);
	const bool maxTrkPassed = (m_maxNtrks == -1) or (countForConfiguredPtThreshold < m_maxNtrks);

	if ( minTrkPassed and maxTrkPassed ) {		
		addDecisionID(m_decisionId.numeric(), trkinfo.decision);
		ATH_MSG_DEBUG("REGTEST event accepted");
	}
	return StatusCode::SUCCESS;
}
