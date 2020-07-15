/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGACCELEVENT_WORKFACTORY_H
#define TRIGACCELEVENT_WORKFACTORY_H

#include <iostream>
#include <vector>
#include <memory>

#include "Work.h"

namespace TrigAccel {

  class WorkFactory{
  public:
    WorkFactory(){};
    virtual ~WorkFactory(){};
    virtual bool configure()  = 0;
    virtual Work* createWork(int, std::shared_ptr<OffloadBuffer> data) = 0;
    virtual const std::vector<int> getProvidedAlgs() = 0;
    virtual int getFactoryId() = 0;
  };
  
}

#endif
