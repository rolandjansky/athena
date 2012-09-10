/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRBCIDSETTINGS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRBCIDSETTINGS_H

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

class L1CaloPprBcidSettings {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloPprBcidSettings& r);
public:

    L1CaloPprBcidSettings();
    L1CaloPprBcidSettings( unsigned int channelId, unsigned short extBcidThreshold,
	                    unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel,
	                    unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned int firStartBit,
	                    unsigned int bcidDecision1, unsigned int satOverride1, unsigned int bcidDecision2, unsigned int satOverride2,
	                    unsigned int bcidDecision3, unsigned int satOverride3, unsigned int peakFinderCond, unsigned int decisionSource );

    L1CaloPprBcidSettings( const L1CaloCoolChannelId& channelId, unsigned short extBcidThreshold,
	                    unsigned short satBcidThreshLow, unsigned short satBcidThreshHigh, unsigned short satBcidLevel,
	                    unsigned short bcidEnergyRangeLow, unsigned short bcidEnergyRangeHigh, unsigned int firStartBit,
	                    unsigned int bcidDecision1, unsigned int satOverride1, unsigned int bcidDecision2, unsigned int satOverride2,
	                    unsigned int bcidDecision3, unsigned int satOverride3, unsigned int peakFinderCond, unsigned int decisionSource );

	virtual ~L1CaloPprBcidSettings() {};

	// getters
	inline L1CaloCoolChannelId channelId(void) const { return m_channelId; };

	inline unsigned short extBcidThreshold() const { return m_extBcidThreshold; };
	inline unsigned short satBcidThreshLow() const { return m_satBcidThreshLow; };
	inline unsigned short satBcidThreshHigh() const { return m_satBcidThreshHigh; };
	inline unsigned short satBcidLevel() const { return m_satBcidLevel; };

	inline unsigned short bcidEnergyRangeLow() const { return m_bcidEnergyRangeLow; };
	inline unsigned short bcidEnergyRangeHigh() const { return m_bcidEnergyRangeHigh; };

	inline unsigned int firStartBit() const { return m_firStartBit; };

	inline unsigned int bcidDecision1() const { return m_bcidDecision1; };
	inline unsigned int satOverride1() const { return m_satOverride1; };

	inline unsigned int bcidDecision2() const { return m_bcidDecision2; };
	inline unsigned int satOverride2() const { return m_satOverride2; };

	inline unsigned int bcidDecision3() const { return m_bcidDecision3; };
	inline unsigned int satOverride3() const { return m_satOverride3; };

	inline unsigned int peakFinderCond() const { return m_peakFinderCond; };
	inline unsigned int decisionSource() const { return m_decisionSource; };

private:

	L1CaloCoolChannelId m_channelId;

	unsigned short m_extBcidThreshold;

	unsigned short m_satBcidThreshLow;
	unsigned short m_satBcidThreshHigh;
	unsigned short m_satBcidLevel;

	unsigned short m_bcidEnergyRangeLow;
	unsigned short m_bcidEnergyRangeHigh;

	unsigned int m_firStartBit;

	unsigned int m_bcidDecision1;
	unsigned int m_satOverride1;
	unsigned int m_bcidDecision2;
	unsigned int m_satOverride2;
	unsigned int m_bcidDecision3;
	unsigned int m_satOverride3;

	unsigned int m_peakFinderCond;
	unsigned int m_decisionSource;

};

#endif
