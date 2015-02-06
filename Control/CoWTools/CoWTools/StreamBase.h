//  ---*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_STREAMBASE_H
#define COWTOOLS_STREAMBASE_H

namespace CoWTools{
  class CoWRecordStats;
  class StreamBase{
  public:
    StreamBase(){};
    virtual ~StreamBase(){};
    virtual void putResults(const CoWRecordStats&)=0;
  };
  
}
#endif
