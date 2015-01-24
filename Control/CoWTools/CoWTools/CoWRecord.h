// --*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_RECORD
#define COWTOOLS_RECORD
#include "CoWTools/CoWRecordStats.h"

namespace CoWTools{
  class CoWRecord{
  public:
    CoWRecord(bool summary=true):ms(summary),m_summary(summary){}
    unsigned long addrStart,addrEnd;
    short perms;
    unsigned long long offset;
    unsigned long inode;
    CoWRecordStats ms;
    unsigned int dmin,dmaj;
    void parseRecord(std::istream& in);
  private:
    bool m_summary;
  };
}//end namespace

#endif
