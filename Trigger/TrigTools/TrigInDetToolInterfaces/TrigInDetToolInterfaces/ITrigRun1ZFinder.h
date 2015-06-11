/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef __ITRIGRUN1ZFINDER_H__ 
#define __ITRIGRUN1ZFINDER_H__ 

#include <vector>
#include "GaudiKernel/IAlgTool.h"
#include "TrigInDetEvent/TrigVertexCollection.h"

class TrigSiSpacePoint;
class IRoiDescriptor;

static const InterfaceID IID_ITrigRun1ZFinder("ITrigRun1ZFinder", 1 , 0); 

class ITrigRun1ZFinder: virtual public IAlgTool 
{
 public:

  static const InterfaceID& interfaceID() {
    return IID_ITrigRun1ZFinder;
  }
    virtual TrigVertexCollection* findZ( const std::vector<const TrigSiSpacePoint *>& spVec, const IRoiDescriptor& RoI) = 0;
};

#endif


