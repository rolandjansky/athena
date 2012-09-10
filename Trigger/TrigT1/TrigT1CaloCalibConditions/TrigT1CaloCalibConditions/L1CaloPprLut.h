/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_L1CALOPPRLUT_H
#define TRIGT1CALOCALIBCONDITIONS_L1CALOPPRLUT_H

#include <iostream>
#include <vector>

#include "CoralBase/Blob.h"

#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"
#include "TrigT1CaloCalibConditions/L1CaloCoolChannelId.h"


/**
 *  Transient class defining the PPM lookup table objects
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */


class L1CaloPprLut{
    friend std::ostream& operator<<(std::ostream& output, const L1CaloPprLut& ppmLut);

public:
    L1CaloPprLut();
    L1CaloPprLut(unsigned int channelId, unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutSlope, unsigned short lutNoiseCut, coral::Blob lutParameters, unsigned int pedValue, double pedMean, ChanCalibErrorCode errorCode);
    L1CaloPprLut(const L1CaloCoolChannelId& channelId, unsigned short lutStrategy, unsigned short lutOffset, unsigned short lutSlope, unsigned short lutNoiseCut, coral::Blob lutParameters, unsigned int pedValue, double pedMean, ChanCalibErrorCode errorCode);

	virtual ~L1CaloPprLut() {};

	// getters
	inline L1CaloCoolChannelId channelId(void) const { return m_channelId; };
	inline unsigned short lutStrategy(void) const { return m_lutStrategy; };
	inline unsigned short lutOffset(void) const { return m_lutOffset; };
	inline unsigned short lutSlope(void) const { return m_lutSlope; };
	inline unsigned short lutNoiseCut(void) const { return m_lutNoiseCut; };
	inline coral::Blob lutParameters(void) const { return m_lutParameters; };

	inline unsigned int pedValue(void) const { return m_pedValue; };
	inline double pedMean(void) const { return m_pedMean; }
	inline ChanCalibErrorCode errorCode(void) const { return m_errorCode; };

private:
	L1CaloCoolChannelId m_channelId;

	unsigned short m_lutStrategy;
	unsigned short m_lutOffset;
	unsigned short m_lutSlope;
	unsigned short m_lutNoiseCut;
	coral::Blob m_lutParameters;

	unsigned int m_pedValue;
	double m_pedMean;

	ChanCalibErrorCode m_errorCode;
};

#endif
