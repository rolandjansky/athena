/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/L1CaloPprFir.h"

#include <string>
#include <sstream>
#include <iostream>

L1CaloPprFir::L1CaloPprFir():
	m_channelId(0),
	m_firStartBit(0)
{
}

L1CaloPprFir::L1CaloPprFir( unsigned int channelId, unsigned int firStartBit, int firCoeff1, int firCoeff2, int firCoeff3, int firCoeff4, int firCoeff5 ):
	m_channelId(channelId),
	m_firStartBit(firStartBit)
{
	m_vFirCoefficients.reserve(5);
	m_vFirCoefficients.push_back(firCoeff1);
	m_vFirCoefficients.push_back(firCoeff2);
	m_vFirCoefficients.push_back(firCoeff3);
	m_vFirCoefficients.push_back(firCoeff4);
	m_vFirCoefficients.push_back(firCoeff5);
}

L1CaloPprFir::L1CaloPprFir( const L1CaloCoolChannelId& channelId, unsigned int firStartBit, int firCoeff1, int firCoeff2, int firCoeff3, int firCoeff4, int firCoeff5 ):
	m_channelId(channelId),
	m_firStartBit(firStartBit)
{
	m_vFirCoefficients.reserve(5);
	m_vFirCoefficients.push_back(firCoeff1);
	m_vFirCoefficients.push_back(firCoeff2);
	m_vFirCoefficients.push_back(firCoeff3);
	m_vFirCoefficients.push_back(firCoeff4);
	m_vFirCoefficients.push_back(firCoeff5);
}

std::ostream& operator<<(std::ostream& output, const  L1CaloPprFir& r) {
	output << "channelID: " << std::hex << r.m_channelId.id() << std::dec << ", " << "firStarBit: " << r.m_firStartBit << ", " << "firCoeff1: " << r.m_vFirCoefficients.at(0) << ", "<<"firCoeff2: " << r.m_vFirCoefficients.at(1) << ", "<<"firCoeff3: " << r.m_vFirCoefficients.at(2) << ", "<<"firCoeff4: " << r.m_vFirCoefficients.at(3) << ", "<<"firCoeff5: " << r.m_vFirCoefficients.at(4);
    return output;
}
