/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDEADMAPCONDDATA_H
#define PIXELDEADMAPCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

class PixelDeadMapCondData {
  public:
    PixelDeadMapCondData();
    virtual ~PixelDeadMapCondData();

    void setModuleStatus(const int chanNum, const int value);
    int getModuleStatus(const int chanNum) const;

    void setChipStatus(const int chanNum, const int value);
    int getChipStatus(const int chanNum) const;

    void clear();

  private:
    typedef std::map<int, int> IntConditions;
    IntConditions  m_moduleStatus;
    IntConditions  m_chipStatus;

};

CLASS_DEF( PixelDeadMapCondData , 395912873 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelDeadMapCondData, 598918393 );

#endif
