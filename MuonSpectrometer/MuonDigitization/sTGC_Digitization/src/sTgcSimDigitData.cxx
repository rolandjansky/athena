/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "sTGC_Digitization/sTgcSimDigitData.h"


// Constructor
sTgcSimDigitData::sTgcSimDigitData() { }

sTgcSimDigitData::sTgcSimDigitData(const sTgcSimDigitData& simDigit) {

    m_sTGCSimData = simDigit.getSimData();
    m_sTGCDigit = simDigit.getSTGCDigit();
}

sTgcSimDigitData::sTgcSimDigitData(const MuonSimData& hit, const sTgcDigit& digit): 
    m_sTGCSimData(hit),
    m_sTGCDigit(digit)
{
}

//sTgcSimDigitData::sTgcSimDigitData(std::vector<GenericMuonSimHit> hitColl, std::vector<sTgcDigit> digitColl) {
//
//    m_sTGCHitColl.addHitCollection(hitColl);
//    m_sTGCDigitColl.addDigitCollection(digitColl);
//}

// Destructor
sTgcSimDigitData::~sTgcSimDigitData() {}

