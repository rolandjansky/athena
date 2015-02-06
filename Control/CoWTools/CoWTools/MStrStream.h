// --*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_MSTRSTREAM_H
#define COWTOOLS_MSTRSTREAM_H
#include "CoWTools/StreamBase.h"
#include <string>
#include "GaudiKernel/MsgStream.h"

namespace CoWTools{
  class CoWRecordStats;
  class MStrStream:public StreamBase{
  public:
    MStrStream(MsgStream &out):m_out(out){}
    ~MStrStream(){};
    virtual void putResults(const CoWRecordStats&);
  private:
    MsgStream &m_out;
  };
}

#endif
