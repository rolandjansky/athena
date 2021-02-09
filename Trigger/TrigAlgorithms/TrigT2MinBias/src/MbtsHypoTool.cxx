
/*
Copyright (C) 2002-2021 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCompositeUtils/HLTIdentifier.h"
#include "AthenaMonitoringKernel/Monitored.h"

#include "MbtsHypoTool.h"

using namespace TrigCompositeUtils;
MbtsHypoTool::MbtsHypoTool(const std::string &type, const std::string &name, const IInterface *parent)
	: AthAlgTool(type, name, parent),
	  m_decisionId(HLT::Identifier::fromToolName(name)) {}

StatusCode MbtsHypoTool::initialize()
{
	return StatusCode::SUCCESS;
}

StatusCode MbtsHypoTool::decide(MbtsHypoInfo &info) const
{
	ATH_MSG_DEBUG("Executing this T2MbtsHypo " << name());

	if (info.previousDecisionIDs.count(m_decisionId.numeric()) == 0)
	{
		ATH_MSG_DEBUG("Already rejected");
		return StatusCode::SUCCESS;
	}

	// Calculate MBTS counter multiplicities after energy and an optional time cut.
	Counts counts = calculateMultiplicities(info.mbtsBits);
	if (counts.sideA == 0 && counts.sideC == 0)
	{
		ATH_MSG_DEBUG("calculateMultiplicities failed, no multiplicities");
		return StatusCode::SUCCESS;
	}
	bool pass = false;
	if (m_coincidence)
	{ // Coincidence logic
		if (!m_veto)
		{
			if (counts.sideA >= m_mbtsCounters && counts.sideC >= m_mbtsCounters)
				pass = true;
		}
		else
		{
			if (counts.sideA < m_mbtsCounters && counts.sideC < m_mbtsCounters)
				pass = true;
		}
	}
	else
	{
		if (m_or)
		{ // Or logic
			if (!m_veto)
			{
				if ((counts.sideA >= m_mbtsCounters || counts.sideC >= m_mbtsCounters))
					pass = true;
			}
			else
			{
				if ((counts.sideA < m_mbtsCounters || counts.sideC < m_mbtsCounters))
					pass = true;
			}
		}
		else
		{ // Sum logic
			if (!m_veto)
			{
				if ((counts.sideA + counts.sideC) >= m_mbtsCounters)
					pass = true;
			}
			else
			{
				if ((counts.sideA + counts.sideC) < m_mbtsCounters)
					pass = true;
			}
		}
	}

	ATH_MSG_DEBUG("REGTEST: event " << (pass ? "accepted" : "failed") << " with EBA(" << counts.sideA << ") + EBC(" << counts.sideC << ") hits above threshold.");

	if (pass)
	{
		addDecisionID(m_decisionId.numeric(), info.decision);
		ATH_MSG_DEBUG("REGTEST event accepted");
	}
	return StatusCode::SUCCESS;
}

MbtsHypoTool::Counts MbtsHypoTool::calculateMultiplicities(const xAOD::TrigT2MbtsBits *t2mbtsBits) const
{

	if (!t2mbtsBits)
	{
		return {0, 0};
	}

	ATH_MSG_DEBUG("Getting energy and time values.");

	std::vector<float> triggerEnergies = t2mbtsBits->triggerEnergies();
	std::vector<float> triggerTimes = t2mbtsBits->triggerTimes();

	if (triggerEnergies.size() != xAOD::TrigT2MbtsBits::NUM_MBTS || triggerTimes.size() != xAOD::TrigT2MbtsBits::NUM_MBTS)
	{
		ATH_MSG_WARNING("Vector sizes are not equal to number of MBTS counters.");
		return {0, 0};
	}

	ATH_MSG_DEBUG("Forming hit multiplicities.");

	std::bitset<16> ebaTriggerBits;
	std::bitset<16> ebcTriggerBits;
	unsigned ibit;
	int ebaCounters = 0, ebcCounters = 0;

	// Initialize monitoring variables.
	double timeDiff_A_C = 0.;
	double timeMean_A = 0.;
	double timeMean_C = 0.;

	// Loop over each counter and form bit sets from time and energy (optional).
	for (ibit = 0; ibit < xAOD::TrigT2MbtsBits::NUM_MBTS; ibit++)
	{
		if (m_mbtsmode == 1 && !(ibit < 8 || (ibit >= 16 && ibit < 24)))
			continue; // count only inner
		if (m_mbtsmode == 2 && !((ibit >= 8 && ibit < 16) || (ibit >= 24 && ibit < 32)))
			continue; // count only outer

		// Check the energy threshold.
		if (triggerEnergies[ibit] <= m_threshold)
			continue;

		// Calculate the mean time for this side;
		if (ibit < 16)
		{
			timeMean_A += triggerTimes[ibit];
			ebaCounters++;
		}
		else
		{
			timeMean_C += triggerTimes[ibit];
			ebcCounters++;
		}

		// Check the time cut if it is active.
		if (m_timeCut >= 0 && std::abs(triggerTimes[ibit] - m_timeOffsets[ibit] - m_globalTimeOffset) >= m_timeCut)
			continue;

		// Set the bits for this side;
		if (ibit < 16)
		{
			ebaTriggerBits.set(ibit);
		}
		else
		{
			ebcTriggerBits.set(ibit - 16);
		}
	}

	if (ebaCounters > 0)
	{
		timeMean_A /= ebaCounters;
	}
	else
	{
		timeMean_A = -999.0;
	}

	if (ebcCounters > 0)
	{
		timeMean_C /= ebcCounters;
	}
	else
	{
		timeMean_C = -999.0;
	}

	if (ebaCounters > 0 && ebcCounters > 0)
	{
		timeDiff_A_C = timeMean_A - timeMean_C;
	}
	else
	{
		timeDiff_A_C = -999.0;
	}
	ATH_MSG_DEBUG("average MBTS trigger time"
				  << " side A: " << timeMean_A
				  << ", side C: " << timeMean_C
				  << ", difference A-C: " << timeDiff_A_C);

	ATH_MSG_DEBUG("MBTS EBA trigger bits: " << ebaTriggerBits);
	ATH_MSG_DEBUG("MBTS EBC trigger bits: " << ebcTriggerBits);

	return {ebaTriggerBits.count(), ebcTriggerBits.count()};
}
