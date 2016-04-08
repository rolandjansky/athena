/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "CoWTools/CoWLibrary.h"
#include <memory>

namespace CoWTools{
  void CoWLibrary::parseRecord(std::istream &in){
    //std::cout<<"Got record line "<<line<<std::endl;
    auto mr=std::make_shared<CoWRecord>(m_summary);
    mr->parseRecord(in);
    m_ms+=mr->m_ms;
  }

  // std::ostream & operator<<(std::ostream &out, const CoWLibrary &l){
  //   out<<"library = "<<l.m_libName<<std::endl;
  //   out<<l.m_ms<<std::endl;
  //   out<<"-------------------------------------------------------"<<std::endl;
  //   return out;
  // }

  std::shared_ptr<CoWLibrary> CoWLibrary::fromRecord(std::istream &in, bool summary){
    std::string libLine;
    if(std::getline(in,libLine).eof()){
      //std::cerr<<libLine<<std::endl;
      std::cerr<<"Returning 0"<<std::endl;
      return 0;
    }
    int len=0;
    int nfields=0;
    std::string libName="Anonymous";
    char buff[2048];
    //std::cerr<<"LibLine="<<libLine<<std::endl;
    sscanf(libLine.c_str(),"%*s %*s %*s %*s %*s%n",
	   &len
	   );
    if(libLine.size()>(uint)(len+1)){
      nfields=sscanf(libLine.substr(len+1).c_str(),"%2047s",buff);
      if(nfields>0){
	libName=buff;
	//std::cout<<"Found lib "<<libName<<std::endl;
      }
    }else{
      libName="Anonymous";
    }
    auto lib=std::make_shared<CoWTools::CoWLibrary>(libName,summary);
    CoWRecordStats r(summary);
    r.parseRecord(in);
    lib->m_ms+=r;
    return lib;
  }
  
  std::ostream & operator<<(std::ostream &os, const CoWTools::CoWLibrary &l){
    // char buff[1024];
    // snprintf(buff,1024,"Library= %s"
    // 	     );
    // out<<buff;

    os<<" Lib= "<<l.m_libName<<" "<<l.m_ms;
    return os;
  }
}
