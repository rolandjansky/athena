/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __LArVCalibHitMergerFactory_H__
#define __LArVCalibHitMergerFactory_H__

#include "CLIDSvc/CLASS_DEF.h"
#include "LArG4Code/LArVCalibHitMerger.h"

class LArVCalibHitMergerFactory 
{
public:

  virtual ~LArVCalibHitMergerFactory() {};

  virtual LArVCalibHitMerger* getHitMerger() const = 0;
};

CLASS_DEF( LArVCalibHitMergerFactory , 219299706 , 1 )

#endif // _LArVCalibHitMergerFactory_H_
