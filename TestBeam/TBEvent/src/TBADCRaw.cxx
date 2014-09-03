/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TBEvent/TBADCRaw.h"
#include <string>

TBADCRaw::TBADCRaw(const std::string& adcID, bool overflow, signal_type theADC)
    : TBBeamDetector(adcID, overflow)
    , m_adc(theADC)
    { }

TBADCRaw::~TBADCRaw(){}
