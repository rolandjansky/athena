/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/FineTimeErrorCode.h"

#include <iomanip>
#include <iostream>
#include <sstream>


FineTimeErrorCode::FineTimeErrorCode() : AbstractErrorCode(0) {
}

FineTimeErrorCode::FineTimeErrorCode(unsigned int errorCode): AbstractErrorCode(errorCode) {
}

FineTimeErrorCode::FineTimeErrorCode(const FineTimeErrorCode& e) : AbstractErrorCode (e.errorCode()) {
}

FineTimeErrorCode& FineTimeErrorCode::operator=(const FineTimeErrorCode& e)
{
	this->errorCode(e.errorCode());
	return *this;
}

FineTimeErrorCode& FineTimeErrorCode::operator|=(const FineTimeErrorCode& e)
{
	this->errorCode( this->errorCode() | e.errorCode() );
	return *this;
}

std::string FineTimeErrorCode::printStatus() const {

	std::stringstream status;

	status << "ErrorCode: " << this->errorCode() << std::endl;

	status << "Bit " << std::setw(2) << BadRefBit
		 << ": BadRefBit:                   "
		 << bitIsSet(BadRefBit) << std::endl;

	status << "Bit " << std::setw(2) << BadCalibBit
		 << ": BadCalibBit:                   "
		 << bitIsSet(BadCalibBit) << std::endl;

	status << "Bit " << std::setw(2) << RefSetByHand
		 << ": RefSetByHand:                   "
		 << bitIsSet(RefSetByHand) << std::endl;
	status << "Bit " << std::setw(2) << CalibSetByHand
		 << ": CalibSetByHand:                   "
		 << bitIsSet(CalibSetByHand) << std::endl;

	return status.str();
}


std::ostream& operator<<(std::ostream& output, const  FineTimeErrorCode& r) {
	output << r.errorCode();
    return output;
}
