/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGZFINDER_H__ 
#define __ITRIGZFINDER_H__ 

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

class TrigSiSpacePointBase;
class IRoiDescriptor;

static const InterfaceID IID_ITrigZFinder("ITrigZFinder", 1 , 0); 

class ITrigZFinder: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigZFinder;
  }
    virtual TrigVertexCollection* findZ( const std::vector<TrigSiSpacePointBase>& spVec, const IRoiDescriptor& RoI) = 0;
};

#endif


