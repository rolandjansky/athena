/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelDCSTempData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel temperature data in PixelDCSTempData.
 */

#ifndef PIXELDCSTEMPDATA_H
#define PIXELDCSTEMPDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <unordered_map>

#include "AthenaKernel/CondCont.h"

class PixelDCSTempData {
  public:
    void setTemperature(const int chanNum, const float value);
    float getTemperature(const int chanNum) const;

  private:
    typedef std::unordered_map<int, float> FloatConditions;
    FloatConditions  m_temperature;
};

CLASS_DEF( PixelDCSTempData , 345932822 , 1 )

CONDCONT_DEF( PixelDCSTempData, 578988322 );

#endif
