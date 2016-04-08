// --*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_VECSTREAM_H
#define COWTOOLS_VECSTREAM_H

#include "CoWTools/StreamBase.h"
#include <string>
#include <vector>

namespace CoWTools{
  class CoWRecordStats;
  class VecStream:public StreamBase{
  public:
    VecStream(std::vector<long> &out):m_out(out){}
    ~VecStream(){
    };
    virtual void putResults(const CoWRecordStats&);
  private:
    std::vector<long> &m_out;
  };
}

#endif
