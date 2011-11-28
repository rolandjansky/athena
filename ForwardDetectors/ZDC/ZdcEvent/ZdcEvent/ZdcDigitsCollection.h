/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef ZdcDigitsCollection_H
#define ZdcDigitsCollection_H

// STL include
#include <vector>

// Lang include
#include <inttypes.h>

#include "ZdcEvent/ZdcDigits.h"
#include "DataModel/DataVector.h"

#include "CLIDSvc/CLASS_DEF.h"

class ZdcDigitsCollection : public DataVector<ZdcDigits>
{

  //friend class ZdcDigitsContainerCnv;
public:

  ZdcDigitsCollection ( SG::OwnershipPolicy ownPolicy=SG::OWN_ELEMENTS )
    : DataVector<ZdcDigits>(ownPolicy) {}

  virtual ~ZdcDigitsCollection() { }

  /**
   * Clear everything for next event
   */
  //virtual void clear ();

  void print();

 private:

};

SG_BASE(ZdcDigitsCollection, DataVector<ZdcDigits>);
CLASS_DEF(ZdcDigitsCollection,929261,0)

#endif

