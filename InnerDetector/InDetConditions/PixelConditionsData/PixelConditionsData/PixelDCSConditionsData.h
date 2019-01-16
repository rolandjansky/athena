/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDCSCONDDATA_H
#define PIXELDCSCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include <map>

class PixelDCSConditionsData {
  public:
    PixelDCSConditionsData();
    virtual ~PixelDCSConditionsData();

    void setValue(const int chanNum, const float value);
    bool getValue(const int chanNum, float& value) const;
    void clear();

  private:
    typedef std::map<int, float> FloatConditions;
    FloatConditions  m_channelValues;
};

CLASS_DEF( PixelDCSConditionsData , 345532773 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelDCSConditionsData, 578786393 );

#endif // PIXELDCSCONDDATA_H
