/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZdcRawChannelCollection_H
#define ZdcRawChannelCollection_H

// STL include
#include <vector>

// Lang include
#include <inttypes.h>

#include "ZdcEvent/ZdcRawChannel.h"
#include "DataModel/DataVector.h"

#include "CLIDSvc/CLASS_DEF.h"

class ZdcRawChannelCollection : public DataVector<ZdcRawChannel>
{

  //friend class ZdcDigitsContainerCnv;
public:

  ZdcRawChannelCollection ( SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS ) 
    : DataVector<ZdcRawChannel>(ownPolicy) {}

  virtual ~ZdcRawChannelCollection() { }

  /**
   * Clear everything for next event
   */
  //virtual void clear ();

  void print(){};

 private:

};

SG_BASE(ZdcRawChannelCollection, DataVector<ZdcRawChannel>);
CLASS_DEF(ZdcRawChannelCollection,92926111,0)

#endif

