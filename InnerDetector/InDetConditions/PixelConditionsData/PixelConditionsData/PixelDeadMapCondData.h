/*
  Copyright (C) 2002-2022 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDEADMAPCONDDATA_H
#define PIXELDEADMAPCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

class PixelDeadMapCondData {
  public:
    typedef std::unordered_map<int, int> IntConditions;
    PixelDeadMapCondData();
    virtual ~PixelDeadMapCondData();

    void setModuleStatus(const int chanNum, const int value);
    int getModuleStatus(const int chanNum) const;
    const IntConditions  &moduleStatusMap() const { return m_moduleStatus; }

    void setChipStatus(const int chanNum, const int value);
    int getChipStatus(const int chanNum) const;

    void clear();

  private:
    IntConditions  m_moduleStatus;
    IntConditions  m_chipStatus;

};

CLASS_DEF( PixelDeadMapCondData , 395912873 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelDeadMapCondData, 598918393 );

#endif
