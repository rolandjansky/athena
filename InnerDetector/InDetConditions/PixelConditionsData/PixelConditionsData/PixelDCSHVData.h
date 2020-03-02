/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelDCSHVData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel HV data in PixelDCSHVData.
 */

#ifndef PIXELDCSHVDATA_H
#define PIXELDCSHVDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <unordered_map>

#include "AthenaKernel/CondCont.h"

class PixelDCSHVData {
  public:
    void setBiasVoltage(const int chanNum, const float value);
    float getBiasVoltage(const int chanNum) const;

  private:
    typedef std::unordered_map<int, float> FloatConditions;
    FloatConditions  m_biasVoltage;
};

CLASS_DEF( PixelDCSHVData , 345932813 , 1 )

CONDCONT_DEF( PixelDCSHVData, 578988313 );

#endif
