/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelTDAQData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel TDAQ information to PixelTDAQData.
 */

#ifndef PIXELTDAQDATA_H
#define PIXELTDAQDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <unordered_map>

#include "AthenaKernel/CondCont.h"

class PixelTDAQData {
  public:
    typedef std::unordered_map<int, int> IntConditions;
    void setModuleStatus(const int chanNum, const int value);
    int getModuleStatus(const int chanNum) const;

    const IntConditions &moduleStatusMap() const { return m_moduleStatus; }
  private:
    IntConditions  m_moduleStatus;
};

CLASS_DEF( PixelTDAQData , 345932866 , 1 )

CONDCONT_DEF( PixelTDAQData, 578988366 );

#endif
