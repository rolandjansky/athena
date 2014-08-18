/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ATHENABARCODE_P1_H
#define ATHENABARCODE_P1_H

#include "Navigation/IAthenaBarCode.h"

class AthenaBarCode_p1 {
public:
	AthenaBarCode_p1():m_athenabarcode(IAthenaBarCode::UNDEFINEDBARCODE) {}
	~AthenaBarCode_p1() {}
	AthenaBarCode_t m_athenabarcode;
};

#endif

