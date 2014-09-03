/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBTDCRaw.h"
#include <string>

TBTDCRaw::TBTDCRaw(const std::string& tdcID, bool overflow,
	     signal_type theTDC, bool theUnderThreshold)
  : TBBeamDetector(tdcID, overflow)
  , m_tdc(theTDC)
  , m_underThreshold(theUnderThreshold)
{ }

TBTDCRaw::~TBTDCRaw(){}
