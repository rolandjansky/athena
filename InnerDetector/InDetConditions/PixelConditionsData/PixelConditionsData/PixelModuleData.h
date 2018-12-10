/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDEADMAPCONDDATA_H
#define PIXELDEADMAPCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>

class PixelModuleData {
  public:
    PixelModuleData();
    virtual ~PixelModuleData();

    void setModuleStatus(const CondAttrListCollection::ChanNum& chanNum, const int value);
    int getModuleStatus(const CondAttrListCollection::ChanNum& chanNum) const;

    void setChipStatus(const CondAttrListCollection::ChanNum& chanNum, const int value);
    int getChipStatus(const CondAttrListCollection::ChanNum& chanNum) const;

    void setTDAQModuleStatus(const CondAttrListCollection::ChanNum& chanNum, const int value);
    int getTDAQModuleStatus(const CondAttrListCollection::ChanNum& chanNum) const;

    void clear();

  private:
    typedef std::map<CondAttrListCollection::ChanNum, int> IntConditions;
    IntConditions  m_moduleStatus;
    IntConditions  m_chipStatus;
    IntConditions  m_tdaqStatus;
};

CLASS_DEF( PixelModuleData , 345932873 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelModuleData, 578988393 );

#endif
