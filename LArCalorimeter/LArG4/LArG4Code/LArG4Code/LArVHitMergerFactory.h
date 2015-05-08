/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArVHitMergerFactory_H__
#define __LArVHitMergerFactory_H__

#include "CLIDSvc/CLASS_DEF.h"
#include "LArG4Code/LArVHitMerger.h"

class LArVHitMergerFactory 
{
public:

  virtual ~LArVHitMergerFactory() {};

  virtual LArVHitMerger* getHitMerger() const = 0;
};

CLASS_DEF( LArVHitMergerFactory , 262982740 , 1 )

#endif // _LArVHitMergerFactory_H_
