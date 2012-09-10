/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRFIR_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRFIR_H

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <iostream>
#include <vector>

/**
*  Transient conditions class for objects defined
*  by the online framework and retrieved from COOL
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

class L1CaloPprFir {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloPprFir& r);
public:

    L1CaloPprFir();
    L1CaloPprFir( unsigned int channelId, unsigned int firStartBit, int firCoeff1, int firCoeff2, int firCoeff3, int firCoeff4, int firCoeff5);
    L1CaloPprFir( const L1CaloCoolChannelId& channelId, unsigned int firStartBit, int firCoeff1, int firCoeff2, int firCoeff3, int firCoeff4, int firCoeff5);

	virtual ~L1CaloPprFir() {};

	// getters
	inline L1CaloCoolChannelId channelId(void) const { return m_channelId; };
	inline std::vector<int> firCoefficients(void) const { return m_vFirCoefficients; };
	inline unsigned int firStartBit(void) const { return m_firStartBit; };


private:
	L1CaloCoolChannelId m_channelId;
	unsigned int m_firStartBit;
	std::vector<int> m_vFirCoefficients;
};

#endif
