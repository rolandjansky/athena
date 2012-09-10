/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGT1CALOCALIBCONDITIONS_FINETIMEERRORCODE_H
#define TRIGT1CALOCALIBCONDITIONS_FINETIMEERRORCODE_H

#include "TrigT1CaloCalibConditions/AbstractErrorCode.h"

#include <string>

/**
 *  FineTimeErrorCode class
 *  for L1Calo error codes
 *
 *  @author Damien Prieur <damien.prieur@cern.ch>
 */

class FineTimeErrorCode: public AbstractErrorCode {

	friend std::ostream& operator<<(std::ostream& output, const FineTimeErrorCode& r);

public:

	enum FineTimeBits {
	  
		BadRefBit = 0,		//set if reference value couldn't be retrieved
		BadCalibBit = 1, 	//set if calib value couldn't be retrieved
		RefSetByHand = 2,	//set if the reference value is set by hand
		CalibSetByHand = 3 	//set if the calibration value is set by hand
	};

	FineTimeErrorCode();
	FineTimeErrorCode(unsigned int errorCode);
	virtual ~FineTimeErrorCode() {}

	FineTimeErrorCode(const FineTimeErrorCode& e);
	FineTimeErrorCode& operator=(const FineTimeErrorCode& e);
	FineTimeErrorCode& operator|=(const FineTimeErrorCode& e);

	bool badRefValue() const { return bitIsSet(BadRefBit); }
	void badRefValue(bool bad) { setBit(BadRefBit,bad); }

	bool badCalib() const { return bitIsSet(BadCalibBit); }
	void badAdc(bool bad) { setBit(BadCalibBit,bad); }

	bool isRefSetByHand() const { return bitIsSet(RefSetByHand); }
	void refIsSetByHand(bool bad) { setBit(RefSetByHand,bad); }

	bool isCalibSetByHand() const { return bitIsSet(CalibSetByHand); }
	void calibIsSetByHand(bool bad) { setBit(CalibSetByHand,bad); }
	
	std::string printStatus() const;
};

#endif


