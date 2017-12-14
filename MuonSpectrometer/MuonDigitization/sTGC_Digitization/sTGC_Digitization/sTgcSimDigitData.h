/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * Class for storing sTGC hit and digit information
 *
 */

#ifndef STGC_DIGITIZATION_SIMDIGITDATA_H
#define STGC_DIGITIZATION_SIMDIGITDATA_H

//#include "MuonSimEvent/GenericMuonSimHitCollection.h"
//#include "MuonSimEvent/GenericMuonSimHit.h"
#include "MuonSimData/MuonSimData.h"
#include "MuonDigitContainer/sTgcDigit.h"
//#include <vector>


class sTgcSimDigitData 
{
public:
    sTgcSimDigitData();
    sTgcSimDigitData(const sTgcSimDigitData& simDigit);
    sTgcSimDigitData(const MuonSimData& hit, const sTgcDigit& digit);
    ~sTgcSimDigitData();

    /// set SimData
    void setSimData(MuonSimData hit) { m_sTGCSimData = hit; }

    /// Set sTGC digit
    void setSTGCDigit(sTgcDigit digit) { m_sTGCDigit = digit; }

    /// Get the SimData
    MuonSimData getSimData() const { return m_sTGCSimData; }

    /// Get the sTGC digit
    sTgcDigit getSTGCDigit() const { return m_sTGCDigit; }

private:

    MuonSimData m_sTGCSimData;
    sTgcDigit m_sTGCDigit;

};

///// Add a muon hit at the end of the hit collection
//inline void sTgcSimDigitData::addSingleHit(GenericMuonSimHit hit) {
//    m_sTGCHitColl.push_back(hit);
//}
//
///// Replace the hit collection by the new container
//inline void sTgcSimDigitData::addHitCollection(std::vector<GenericMuonSimHit> hitColl) {
//    m_sTGCHitColl = hitColl;
//}
//
///// Add a sTGC digit at the end of the collection
//inline void sTgcSimDigitData::addSingleDigit(sTgcDigit digit) {
//    m_sTGCDigitColl.push_back(digit);
//}
//
///// Replace the sTGC digit collection by the new container
//inline void sTgcSimDigitData::addDigitCollection(std::vector<sTgcDigit> digitColl) {
//    m_sTGCDigitColl = digitColl;
//}
//
///// Get the muon hit collection
//inline std::vector<GenericMuonSimHit> sTgcSimDigitData::getHitCollection() const {
//    return m_sTGCHitColl;
//}
//
///// Get the sTGC digit collection
//inline std::vector<sTgcDigit> sTgcSimDigitData::getDigitCollection() const {
//    return m_sTGCDigitColl;
//}

#endif
