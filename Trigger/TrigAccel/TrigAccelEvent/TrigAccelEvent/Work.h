/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGACCELEVENT_WORK_H
#define TRIGACCELEVENT_WORK_H

#include<memory>

#include "OffloadBuffer.h"

namespace TrigAccel {

  class Work {
    
  public:
    
    Work() {};
    virtual ~Work() {};
    virtual std::shared_ptr<OffloadBuffer> getOutput() = 0;
    virtual bool run() = 0;
    virtual unsigned int getId() const = 0;
    
  };
}

#endif
