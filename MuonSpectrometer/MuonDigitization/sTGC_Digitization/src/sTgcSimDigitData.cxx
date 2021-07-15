/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGC_Digitization/sTgcSimDigitData.h"


// Constructor
sTgcSimDigitData::sTgcSimDigitData() { }


sTgcSimDigitData::sTgcSimDigitData(const MuonSimData& hit, const sTgcDigit& digit): 
    m_sTGCSimData(hit),
    m_sTGCDigit(digit)
{
}


