// --*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_COWLIBRARY
#define COWTOOLS_COWLIBRARY
#include <iostream>
#include <unordered_map>
#include <string>
#include <memory>
#include "CoWTools/CoWRecord.h"

namespace CoWTools{
  class CoWLibrary{
  public:
    CoWLibrary(bool Summary=true):libName("Summary"),m_summary(Summary){};
    CoWLibrary(const std::string &n,bool Summary=true):libName(n),ms(Summary),m_summary(Summary){};
    std::string libName;
    std::unordered_map<std::string,std::shared_ptr<CoWRecord> > records;
    void parseRecord(std::istream& in,const std::string &line);
    CoWRecordStats ms;
  private:
    bool m_summary;
    friend std::ostream & operator<<(std::ostream &, const CoWLibrary &l);
  };

}//end namespace

#endif
