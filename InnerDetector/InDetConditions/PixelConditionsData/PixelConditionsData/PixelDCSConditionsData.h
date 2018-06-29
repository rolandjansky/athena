/*
  Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDCSCONDDATA_H
#define PIXELDCSCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>

class PixelDCSConditionsData {
  public:
    PixelDCSConditionsData();
    virtual ~PixelDCSConditionsData();

    void setValue(const CondAttrListCollection::ChanNum& chanNum, const float value);
    void setValue(const CondAttrListCollection::ChanNum& chanNum, const std::string value);
    bool getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const;
    bool getValue(const CondAttrListCollection::ChanNum& chanNum, std::string& value) const;
    void clear();

  private:
    typedef std::map<CondAttrListCollection::ChanNum, float> FloatConditions;
    typedef std::map<CondAttrListCollection::ChanNum, std::string> StringConditions;
    FloatConditions  m_channelValues;
    StringConditions m_channelStrings;
};

CLASS_DEF( PixelDCSConditionsData , 345532773 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelDCSConditionsData, 578786393 );

#endif // PIXELDCSCONDDATA_H
