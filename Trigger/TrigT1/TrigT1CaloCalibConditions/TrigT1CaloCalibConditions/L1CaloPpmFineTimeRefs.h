/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPMFINETIMEREFS_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPMFINETIMEREFS_H

#include "TrigT1CaloCalibConditions/AbstractL1CaloPersistentCondition.h"
#include "TrigT1CaloCalibConditions/FineTimeErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"

#include <iostream>

/**
*  Transient conditions class for objects defined
*  by the online framework and retrieved from COOL
*
*  @author Damien Prieur <damien.prieur@cern.ch>
*/

class string;

class L1CaloPpmFineTimeRefs {
	friend std::ostream& operator<<(std::ostream& output, const L1CaloPpmFineTimeRefs& r);

public:

    L1CaloPpmFineTimeRefs();
    L1CaloPpmFineTimeRefs(const L1CaloCoolChannelId& channelId);
    L1CaloPpmFineTimeRefs(unsigned int channelId, FineTimeErrorCode errorCode , double refValue, double criterion, double calibValue);
    L1CaloPpmFineTimeRefs(const L1CaloCoolChannelId& channelId, FineTimeErrorCode errorCode, double refValue, double criterion, double calibValue);

	virtual ~L1CaloPpmFineTimeRefs() {};

	// getters
	inline L1CaloCoolChannelId channelId(void) const { return m_channelId; };
	inline FineTimeErrorCode errorCode(void) const { return m_errorCode; };
	inline double refValue(void) const { return m_refValue; };
	inline double criterion(void) const { return m_criterion; };
	inline double calibValue(void) const { return m_calibValue; };


private:
	L1CaloCoolChannelId m_channelId;
	FineTimeErrorCode m_errorCode;
	double m_refValue;
	double m_criterion;
	double m_calibValue;
};

#endif
