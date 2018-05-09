/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef PIXELDCSCONDDATA_H
#define PIXELDCSCONDDATA_H

#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaPoolUtilities/CondAttrListCollection.h"
#include <map>

class PixelDCSCondData {
  public:
    //constructor
    PixelDCSCondData();

    //destructor
    virtual ~PixelDCSCondData();
    //@name main methods
    //@{
    /// set a float value for a channel
    void setValue(const CondAttrListCollection::ChanNum& chanNum, const float value);
    /// get the float value for a channel
    bool getValue(const CondAttrListCollection::ChanNum& chanNum, float& value) const;
    /// clear the m_channelValues
    void clear();

    void fill(const CondAttrListCollection::ChanNum& chanNum, const std::string param);
    int output(const CondAttrListCollection::ChanNum & chanNum) const;
    //@}

  private:
    typedef std::map<CondAttrListCollection::ChanNum, float> FloatConditions;
    typedef std::map<CondAttrListCollection::ChanNum, std::vector<std::string> > DCSConditions;
    FloatConditions m_channelValues;
    DCSConditions m_bad_channels;
};

CLASS_DEF( PixelDCSCondData , 345532772 , 1 )

#include "AthenaKernel/CondCont.h"
CONDCONT_DEF( PixelDCSCondData, 578786392 );

#endif // PIXELDCSCONDDATA_H
