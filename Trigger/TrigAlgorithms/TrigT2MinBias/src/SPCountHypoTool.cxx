
/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "SPCountHypoTool.h"

using namespace TrigCompositeUtils;
SPCountHypoTool::SPCountHypoTool(const std::string &type, const std::string &name, const IInterface *parent)
	: AthAlgTool(type, name, parent),
	  m_decisionId(HLT::Identifier::fromToolName(name)) {}

StatusCode SPCountHypoTool::initialize()
{
	return StatusCode::SUCCESS;
}

bool SPCountHypoTool::applyCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite *composit) const
{
	if (threshold == -1)
		return m_logicAnd; //If the logic is And, it should return true and otherwise it should return false.
	ATH_MSG_DEBUG("count for = " << threshold.name() << "=" << (composit->getDetail<int>(threshold.name()) > threshold));
	return (composit->getDetail<int>(threshold.name()) > threshold);
}

StatusCode SPCountHypoTool::decide(SPCountsInfo &spinfo) const
{
	if (spinfo.previousDecisionIDs.count(m_decisionId.numeric()) == 0)
	{
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}
	std::vector<bool> decisionCuts({applyCut(m_pixCL, spinfo.counts),
									applyCut(m_pixCL_1, spinfo.counts),
									applyCut(m_pixCL_2, spinfo.counts),
									applyCut(m_pixCLmin3, spinfo.counts),
									applyCut(m_pixCLBarrel, spinfo.counts),
									applyCut(m_pixCLEndcapA, spinfo.counts),
									applyCut(m_pixCLEndcapC, spinfo.counts),
									applyCut(m_sctSP, spinfo.counts),
									applyCut(m_sctSPBarrel, spinfo.counts),
									applyCut(m_sctSPEndcapA, spinfo.counts),
									applyCut(m_sctSPEndcapC, spinfo.counts)});

	if (m_logicAnd && !std::all_of(decisionCuts.begin(), decisionCuts.end(), [](bool k) { return k; }))
	{
		return StatusCode::SUCCESS;
	}
	else if (m_logicAnd == false && !std::any_of(decisionCuts.begin(), decisionCuts.end(), [](bool k) { return k; }))
	{
		return StatusCode::SUCCESS;
	}
	else
	{
		addDecisionID(m_decisionId.numeric(), spinfo.decision);
		ATH_MSG_DEBUG("REGTEST event accepted");
	}
	return StatusCode::SUCCESS;
}
