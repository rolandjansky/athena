/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

/** 
 * Class for storing sTGC hit and digit information
 *
 */

#ifndef STGC_DIGITIZATION_SIMDIGITDATA_H
#define STGC_DIGITIZATION_SIMDIGITDATA_H

#include "MuonSimData/MuonSimData.h"
#include "MuonDigitContainer/sTgcDigit.h"


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

#endif
