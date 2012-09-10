/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprConditions.h"

#include <string>
#include <sstream>
#include <iostream>
#include <iomanip>

L1CaloPprConditions::L1CaloPprConditions():
	m_extBcidThreshold(0),
	m_satBcidThreshLow(0),
	m_satBcidThreshHigh(0),
	m_satBcidLevel(0),
	m_bcidEnergyRangeLow(0),
	m_bcidEnergyRangeHigh(0),
	m_firStartBit(0),
	m_lutStrategy(0),
	m_lutOffset(0),
	m_lutSlope(0),
	m_lutNoiseCut(0),
	m_pedValue(0),
	m_pedMean(0.)
{}

L1CaloPprConditions::L1CaloPprConditions( unsigned short extBcidThreshold,
	                    unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel,
	                    unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned short firStartBit,
			    short int firCoeff1, short int firCoeff2, short int firCoeff3, short int firCoeff4, short int firCoeff5,
			    unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutSlope, unsigned short lutNoiseCut,
			    unsigned int pedValue, float pedMean):

	m_extBcidThreshold(extBcidThreshold),
	m_satBcidThreshLow(satBcidThreshLow),
	m_satBcidThreshHigh(satBcidThreshHigh),
	m_satBcidLevel(satBcidLevel),
	m_bcidEnergyRangeLow(bcidEnergyRangeLow),
	m_bcidEnergyRangeHigh(bcidEnergyRangeHigh),
	m_firStartBit(firStartBit),
	m_lutStrategy(lutStrategy),
	m_lutOffset(lutOffset),
	m_lutSlope(lutSlope),
	m_lutNoiseCut(lutNoiseCut),
	m_pedValue(pedValue),
	m_pedMean(pedMean)
{
	m_vFirCoefficients.reserve(5);
	m_vFirCoefficients.push_back(firCoeff1);
	m_vFirCoefficients.push_back(firCoeff2);
	m_vFirCoefficients.push_back(firCoeff3);
	m_vFirCoefficients.push_back(firCoeff4);
	m_vFirCoefficients.push_back(firCoeff5);
}

std::ostream& operator<<(std::ostream& output, const  L1CaloPprConditions& r) {
	output << "extBcidThreshold: "    << r.m_extBcidThreshold       << ", "
	       << "satBcidThreshLow: "    << r.m_satBcidThreshLow       << ", "
	       << "satBcidThreshHigh: "   << r.m_satBcidThreshHigh      << ", "
	       << "bcidEnergyRangeLow: "  << r.m_bcidEnergyRangeLow     << ", "
	       << "bcidEnergyRangeHigh: " << r.m_bcidEnergyRangeHigh    << std::endl
	       << "firStartBit: "         << r.m_firStartBit            << ", "
	       << "firCoeff1: "           << r.m_vFirCoefficients.at(0) << ", "
	       << "firCoeff2: "           << r.m_vFirCoefficients.at(1) << ", "
	       << "firCoeff3: "           << r.m_vFirCoefficients.at(2) << ", "
	       << "firCoeff4: "           << r.m_vFirCoefficients.at(3) << ", "
	       << "firCoeff5: "           << r.m_vFirCoefficients.at(4) << std::endl
	       << "lutStrategy: "         << r.m_lutStrategy            << ", "
	       << "lutOffset: "           << r.m_lutOffset              << ", "
	       << "lutSlope: "            << r.m_lutSlope               << ", "
	       << "lutNoiseCut: "         << r.m_lutNoiseCut            << ", "
	       << "pedValue: "            << r.m_pedValue               << ", "
	       << "pedMean: "             << r.m_pedMean                << std::endl;
    return output;
}
