/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/ChanDeadErrorCode.h"

#include <iomanip>
#include <iostream>
#include <sstream>


ChanDeadErrorCode::ChanDeadErrorCode() : AbstractErrorCode(0) {
}

ChanDeadErrorCode::ChanDeadErrorCode(unsigned int errorCode): AbstractErrorCode(errorCode) {
}

ChanDeadErrorCode::ChanDeadErrorCode(const ChanDeadErrorCode& e) : AbstractErrorCode (e.errorCode()) {
}

ChanDeadErrorCode& ChanDeadErrorCode::operator=(const ChanDeadErrorCode& e)
{
	this->errorCode(e.errorCode());
	return *this;
}

ChanDeadErrorCode& ChanDeadErrorCode::operator|=(const ChanDeadErrorCode& e)
{
	this->errorCode( this->errorCode() | e.errorCode() );
	return *this;
}

std::string ChanDeadErrorCode::printStatus() const {

	std::stringstream status;

	status << "ErrorCode: " << this->errorCode() << std::endl;

	status << "Bit " << std::setw(2) << BadAdcBit
		 << ": BadAdcBit:                   "
		 << bitIsSet(BadAdcBit) << std::endl;

	status << "Bit " << std::setw(2) << BadMcmBit
		 << ": BadMcmBit:                   "
		 << bitIsSet(BadMcmBit) << std::endl;

	status << "Bit " << std::setw(2) << FrontEndDeadBit
		 << ": FrontEndDeadBit:             "
		 << bitIsSet(FrontEndDeadBit) << std::endl;

	status << "Bit " << std::setw(2) << ReceiverDeadBit
		 << ": ReceiverDeadBit:             "
		 << bitIsSet(ReceiverDeadBit) << std::endl;

	status << "Bit " << std::setw(2) << HighVoltsOffBit
		 << ": HighVoltsOffBit:             "
		 << bitIsSet(HighVoltsOffBit) << std::endl;

	status << "Bit " << std::setw(2) << LowVoltsOffBit
		 << ": LowVoltsOffBit:              "
		 << bitIsSet(LowVoltsOffBit) << std::endl;

	status << "Bit " << std::setw(2) << NoisyTowerBit
		 << ": NoisyTowerBit:               "
		 << bitIsSet(NoisyTowerBit) << std::endl;

	status << "Bit " << std::setw(2) << PSCellsMaskedBit
		 << ": PSCellsMaskedBit:            "
		 << bitIsSet(PSCellsMaskedBit) << std::endl;

	status << "Bit " << std::setw(2) << FrontCellsMaskedBit
		 << ": FrontCellsMaskedBit:         "
		 << bitIsSet(FrontCellsMaskedBit) << std::endl;

	status << "Bit " << std::setw(2) << MiddleCellsMaskedBit
		 << ": MiddleCellsMaskedBit:        "
		 << bitIsSet(MiddleCellsMaskedBit) << std::endl;

	status << "Bit " << std::setw(2) << BackCellsMaskedBit
		 << ": BackCellsMaskedBit:          "
		 << bitIsSet(BackCellsMaskedBit) << std::endl;

	return status.str();
}


std::ostream& operator<<(std::ostream& output, const  ChanDeadErrorCode& r) {
	output << r.errorCode();
    return output;
}
