//  ---*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_MONITOR_H
#define COWTOOLS_MONITOR_H

#include "GaudiKernel/MsgStream.h"
#include <vector>
#include "CoWTools/CoWRecordStats.h"

namespace CoWTools{
  class StreamBase;
  class Monitor{
  public:
    enum VecPos{VMem=0,RSS=1,PSS=2,Shared=3,Private=4,Anon=5};
    Monitor(std::string &out);
    Monitor(MsgStream &out);
    Monitor(std::vector<long> &out);
    ~Monitor();

    Monitor (const Monitor&) = delete;
    Monitor& operator= (const Monitor&) = delete;
  private:
    const CoWTools::CoWRecordStats parseSmaps() const;
    StreamBase *m_out;
    CoWTools::CoWRecordStats m_stats;
  };
}
#endif
