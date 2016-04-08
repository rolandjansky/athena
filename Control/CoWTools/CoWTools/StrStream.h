// --*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_STRSTREAM_H
#define COWTOOLS_STRSTREAM_H
#include "CoWTools/StreamBase.h"
#include <string>

namespace CoWTools{
  class CoWRecordStats;
  class StrStream:public StreamBase{
  public:
    StrStream(std::string &out):m_out(out){}
    ~StrStream(){};
    virtual void putResults(const CoWRecordStats&);
  private:
    std::string &m_out;
  };
}

#endif
