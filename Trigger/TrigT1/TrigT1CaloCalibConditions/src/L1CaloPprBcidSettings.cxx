/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprBcidSettings.h"

#include <string>
#include <sstream>
#include <iostream>

L1CaloPprBcidSettings::L1CaloPprBcidSettings():
	m_channelId(0),

	m_extBcidThreshold(0),

	m_satBcidThreshLow(0),
	m_satBcidThreshHigh(0),
	m_satBcidLevel(0),

	m_bcidEnergyRangeLow(0),
	m_bcidEnergyRangeHigh(0),

	m_firStartBit(0),

	m_bcidDecision1(0),
	m_satOverride1(0),
	m_bcidDecision2(0),
	m_satOverride2(0),
	m_bcidDecision3(0),
	m_satOverride3(0),

	m_peakFinderCond(0),
	m_decisionSource(0)
{}

L1CaloPprBcidSettings::L1CaloPprBcidSettings( unsigned int channelId, unsigned short extBcidThreshold,
	                    unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel,
	                    unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned int firStartBit,
	                    unsigned int bcidDecision1, unsigned int satOverride1, unsigned int bcidDecision2, unsigned int satOverride2,
	                    unsigned int bcidDecision3, unsigned int satOverride3, unsigned int peakFinderCond, unsigned int decisionSource ):
	m_channelId(channelId),

	m_extBcidThreshold(extBcidThreshold),

	m_satBcidThreshLow(satBcidThreshLow),
	m_satBcidThreshHigh(satBcidThreshHigh),
	m_satBcidLevel(satBcidLevel),

	m_bcidEnergyRangeLow(bcidEnergyRangeLow),
	m_bcidEnergyRangeHigh(bcidEnergyRangeHigh),

	m_firStartBit(firStartBit),

	m_bcidDecision1(bcidDecision1),
	m_satOverride1(satOverride1),
	m_bcidDecision2(bcidDecision2),
	m_satOverride2(satOverride2),
	m_bcidDecision3(bcidDecision3),
	m_satOverride3(satOverride3),

	m_peakFinderCond(peakFinderCond),
	m_decisionSource(decisionSource)
{
}

L1CaloPprBcidSettings::L1CaloPprBcidSettings( const L1CaloCoolChannelId& channelId, unsigned short extBcidThreshold,
	                    unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel,
	                    unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned int firStartBit,
	                    unsigned int bcidDecision1, unsigned int satOverride1, unsigned int bcidDecision2, unsigned int satOverride2,
	                    unsigned int bcidDecision3, unsigned int satOverride3, unsigned int peakFinderCond, unsigned int decisionSource ):
	m_channelId(channelId),

	m_extBcidThreshold(extBcidThreshold),

	m_satBcidThreshLow(satBcidThreshLow),
	m_satBcidThreshHigh(satBcidThreshHigh),
	m_satBcidLevel(satBcidLevel),

	m_bcidEnergyRangeLow(bcidEnergyRangeLow),
	m_bcidEnergyRangeHigh(bcidEnergyRangeHigh),

	m_firStartBit(firStartBit),

	m_bcidDecision1(bcidDecision1),
	m_satOverride1(satOverride1),
	m_bcidDecision2(bcidDecision2),
	m_satOverride2(satOverride2),
	m_bcidDecision3(bcidDecision3),
	m_satOverride3(satOverride3),

	m_peakFinderCond(peakFinderCond),
	m_decisionSource(decisionSource)
{
}

std::ostream& operator<<(std::ostream& output, const  L1CaloPprBcidSettings& r) {
	output << "channelID: " << std::hex << r.m_channelId.id() << std::dec << ", "
	       << "extBcidThreshold: " << r.m_extBcidThreshold << ", "
	       << "satBcidThreshLow: " << r.m_satBcidThreshLow << ", "<<"satBcidThreshHigh: " << r.m_satBcidThreshHigh << ", "
	       << "bcidEnergyRangeLow: " << r.m_bcidEnergyRangeLow << ", " << "bcidEnergyRangeHigh: " << r.m_bcidEnergyRangeHigh << ", "
	       << "firStartBit: " << (int) r.m_firStartBit << ", "
	       << "bcidDecision1: " << r.m_bcidDecision1 << ", "<< "satOverride1: " << r.m_satOverride1 << ", "
	       << "bcidDecision2: " << r.m_bcidDecision2 << ", "<< "satOverride2: " << r.m_satOverride2 << ", "
	       << "bcidDecision3: " << r.m_bcidDecision3 << ", "<< "satOverride3: " << r.m_satOverride3 << ", "
	       << "peakFinderCond: " << r.m_peakFinderCond << ", "<< "decisionSource: " << r.m_decisionSource;
    return output;
}
