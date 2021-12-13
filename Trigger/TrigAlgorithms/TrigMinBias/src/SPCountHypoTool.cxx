
/*
Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "SPCountHypoTool.h"

using namespace TrigCompositeUtils;
SPCountHypoTool::SPCountHypoTool(const std::string &type, const std::string &name, const IInterface *parent)
	: AthCheckedComponent<AthAlgTool>(type, name, parent),
	  m_decisionId(HLT::Identifier::fromToolName(name)) {}

StatusCode SPCountHypoTool::initialize()
{
	return StatusCode::SUCCESS;
}

bool SPCountHypoTool::applyCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite *composit, const std::string_view name) const
{
	if (threshold == -1)
		return m_logicAnd; //If the logic is And, it should return true and otherwise it should return false.
	const std::string detailName =  (name != "" ? std::string(name) : threshold.name() );

	ATH_MSG_DEBUG("count for = " << threshold.name() << "=" << (composit->getDetail<int>(detailName) > threshold));
	return (composit->getDetail<int>(detailName) > threshold);
}

bool SPCountHypoTool::applyInverseCut(const Gaudi::Property<int> &threshold, const xAOD::TrigComposite *composit, const std::string_view name) const
{
	if (threshold == -1)
		return m_logicAnd; //If the logic is And, it should return true and otherwise it should return false.
	const std::string detailName =  (name != "" ? std::string(name) : threshold.name() );

	ATH_MSG_DEBUG("count for = " << threshold.name() << "=" << (composit->getDetail<int>(detailName) > threshold));
	return (composit->getDetail<int>(detailName) < threshold); // here is the difference
}


StatusCode SPCountHypoTool::decide(SPCountsInfo &spinfo) const
{
	if (spinfo.previousDecisionIDs.count(m_decisionId.numeric()) == 0)
	{
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}
	std::vector<bool> decisionCuts({applyCut(m_pixCL, spinfo.counts),
									applyInverseCut(m_pixCLMax, spinfo.counts, "pixCL"),
									applyCut(m_pixCL_1, spinfo.counts),
									applyCut(m_pixCL_2, spinfo.counts),
									applyCut(m_pixCLmin3, spinfo.counts),
									applyCut(m_pixCLBarrel, spinfo.counts),
									applyCut(m_pixCLEndcapA, spinfo.counts),
									applyCut(m_pixCLEndcapC, spinfo.counts),
									applyCut(m_sctSP, spinfo.counts),
									applyInverseCut(m_sctSPMax, spinfo.counts, "sctSP"),
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
