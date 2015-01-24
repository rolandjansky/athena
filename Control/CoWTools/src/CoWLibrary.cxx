/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWTools/CoWLibrary.h"
#include <memory>

namespace CoWTools{
  void CoWLibrary::parseRecord(std::istream &in,const std::string& /*&line*/){
    //std::cout<<"Got record line "<<line<<std::endl;
    auto mr=std::make_shared<CoWRecord>(m_summary);
    mr->parseRecord(in);
    ms+=mr->ms;
  }
  std::ostream & operator<<(std::ostream &out, const CoWLibrary &l){
    out<<"library = "<<l.libName<<std::endl;
    out<<l.ms<<std::endl;
    out<<"-------------------------------------------------------"<<std::endl;
    return out;
  }
}
