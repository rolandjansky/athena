/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/
/**
 * @file PixelConditionsData/PixelDCSStatusData.h
 * @author Soshi Tsuno <Soshi.Tsuno@cern.ch>
 * @date November, 2019
 * @brief Store pixel DCS module status in PixelDCSStatusData.
 */

#ifndef PIXELDCSSTATUSDATA_H
#define PIXELDCSSTATUSDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <unordered_map>

#include "AthenaKernel/CondCont.h"

class PixelDCSStatusData {
  public:
    void setModuleStatus(const int chanNum, const int value);
    int getModuleStatus(const int chanNum) const;

    enum DCSModuleStatus{OK,WARNING,ERROR,FATAL,NOSTATUS};

  private:
    typedef std::unordered_map<int, int> IntConditions;
    IntConditions  m_moduleStatus;
};

CLASS_DEF( PixelDCSStatusData , 345932844 , 1 )

CONDCONT_DEF( PixelDCSStatusData, 578988344 );

#endif
