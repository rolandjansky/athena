/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRCONDITIONS_H

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <iostream>
#include <vector>

/**
*  Transient conditions class for objects defined
*  by the online framework and retrieved from COOL
*
*  @author Peter Faulkner
*/

class L1CaloPprConditions final {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloPprConditions& r);
public:

    L1CaloPprConditions();
    L1CaloPprConditions( unsigned short extBcidThreshold,
	                 unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel,
	                 unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned short firStartBit,
			 short int firCoeff1, short int firCoeff2, short int firCoeff3, short int firCoeff4, short int firCoeff5,
			 unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutSlope, unsigned short lutNoiseCut,
			 unsigned int pedValue, float pedMean);

	~L1CaloPprConditions() = default;

	// getters
	inline unsigned short extBcidThreshold()    const { return m_extBcidThreshold; }
	inline unsigned short satBcidThreshLow()    const { return m_satBcidThreshLow; }
	inline unsigned short satBcidThreshHigh()   const { return m_satBcidThreshHigh; }
	inline unsigned short satBcidLevel()        const { return m_satBcidLevel; }
	inline unsigned short bcidEnergyRangeLow()  const { return m_bcidEnergyRangeLow; }
	inline unsigned short bcidEnergyRangeHigh() const { return m_bcidEnergyRangeHigh; }
	inline unsigned short firStartBit()         const { return m_firStartBit; }
	inline const std::vector<short int>& firCoefficients() const { return m_vFirCoefficients; }
	inline unsigned short lutStrategy()         const { return m_lutStrategy; }
	inline unsigned short lutOffset()           const { return m_lutOffset; }
	inline unsigned short lutSlope()            const { return m_lutSlope; }
	inline unsigned short lutNoiseCut()         const { return m_lutNoiseCut; }
	inline unsigned int   pedValue()            const { return m_pedValue; }
	inline float          pedMean()             const { return m_pedMean; }

private:

	unsigned short m_extBcidThreshold;
	unsigned short m_satBcidThreshLow;
	unsigned short m_satBcidThreshHigh;
	unsigned short m_satBcidLevel;
	unsigned short m_bcidEnergyRangeLow;
	unsigned short m_bcidEnergyRangeHigh;
	unsigned short m_firStartBit;
	std::vector<short int> m_vFirCoefficients;
	unsigned short m_lutStrategy;
	unsigned short m_lutOffset;
	unsigned short m_lutSlope;
	unsigned short m_lutNoiseCut;
        unsigned int m_pedValue;
        float m_pedMean;

};

#endif
