/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigT1CaloCalibConditions/ChanFitErrorCode.h"

ChanFitErrorCode::ChanFitErrorCode() :
  AbstractErrorCode(0)
{}

ChanFitErrorCode::ChanFitErrorCode(unsigned int errorCode) :
  AbstractErrorCode(errorCode)
{}

ChanFitErrorCode::ChanFitErrorCode(const ChanFitErrorCode& e):
  AbstractErrorCode(e.errorCode())
{}

ChanFitErrorCode& ChanFitErrorCode::operator=(const ChanFitErrorCode& rhs) {
    this->errorCode(rhs.errorCode());
    return *this;
}

ChanFitErrorCode& ChanFitErrorCode::operator|=(const ChanFitErrorCode& rhs) {
    this->errorCode(this->errorCode() | rhs.errorCode());
    return *this;
}

/* Channel is valid if no bit is set - to be changed. */
bool ChanFitErrorCode::chanValid() const {
    return (this->errorCode() == 0);
}
