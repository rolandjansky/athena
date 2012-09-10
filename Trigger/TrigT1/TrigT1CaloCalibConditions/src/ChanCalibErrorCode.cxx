/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/ChanCalibErrorCode.h"

#include <iomanip>
#include <iostream>
#include <sstream>


ChanCalibErrorCode::ChanCalibErrorCode() : AbstractErrorCode(0) {
}

ChanCalibErrorCode::ChanCalibErrorCode(unsigned int errorCode): AbstractErrorCode(errorCode) {
}

ChanCalibErrorCode::ChanCalibErrorCode(const ChanCalibErrorCode& e) : AbstractErrorCode(e.errorCode()) {
}

ChanCalibErrorCode& ChanCalibErrorCode::operator=(const ChanCalibErrorCode& e)
{
	this->errorCode(e.errorCode());
	return *this;
}

ChanCalibErrorCode& ChanCalibErrorCode::operator|=(const ChanCalibErrorCode& e)
{
	this->errorCode( this->errorCode() | e.errorCode() );
	return *this;
}


unsigned int ChanCalibErrorCode::errorCode(ChanErrorComponent component) const {
	return (this->errorCode() >> (16 + 4 * component)) & 0xf;
}


//********************************************************************
//              getChanValid
//********************************************************************
/*!
  Checks if channel is valid, ie. none of the lowest four bits set.
*/

bool ChanCalibErrorCode::chanValid() const {
  return ( (this->errorCode() & 0xf) == 0);
}

//********************************************************************
//              getChanValid
//********************************************************************
/*!
  Checks if the component specific part is valid.
*/

bool ChanCalibErrorCode::chanValid(ChanErrorComponent component) const {
	return ( this->errorCode(component) == 0);
}


std::string ChanCalibErrorCode::printStatus(ChanErrorSet spec) const {

	std::stringstream status;

	status << "ErrorCode: " << this->errorCode() << std::endl
		 << "Bit  0: NoDataBit:                    " << noData() << std::endl
		 << "Bit  1: BadDataBit:                   " << badData() << std::endl
		 << "Bit  2: BadResultBit:                 " << badResult() << std::endl
		 << "Bit  3: Not defined:                  " << bitIsSet(4) << std::endl;
	if (spec == PPM) {
	status << "Bit  4: DacScanBadSlopeBit:         " << dacScanBadSlope() << std::endl
		   << "Bit  5: DacScanBadOffsetBit:        " << dacScanBadOffset() << std::endl
		   << "Bit  6: DacScanBadCorrCoeffBit:     " << dacScanBadCorrCoeff() << std::endl
		   << "Bit  7: DacScanBadAdcDistribBit:    " << dacScanBadAdcDistrib() << std::endl

		   << "Bit  9: PedRunBadMeanBit:           " << pedRunBadMean() << std::endl
		   << "Bit 10: PedRunBadSigmaBit:          " << pedRunBadSigma() << std::endl
		   << "Bit 11: PedRunBadAdcDistrib:        " << pedRunBadAdcDistrib() << std::endl;

	}
	else if (spec == CPM) {
	}
	else if (spec == JEM) {
	status << "Bit  5: TotalWindowSizeUndercutBit:   " << totalWindowSizeUndercut() << std::endl
		   << "Bit  6: DistToErrorRegionUndercutBit: " << distToErrorRegionUndercut() << std::endl
		   << "Bit  7: ExceededDelayBit:             " << exceededDelay() << std::endl
		   << "Bit  8: ExceededDelayAfterCorrBit:    " << exceededDelayAfterCorr() << std::endl;
	}
	else if (spec == CMM) {
	}

	return status.str();
}

std::ostream& operator<<(std::ostream& output, const  ChanCalibErrorCode& r) {
	output << r.errorCode();
    return output;
}
