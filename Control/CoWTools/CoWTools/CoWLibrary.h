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
#include <exception>
namespace CoWTools{
  class CoWLibrary{
  public:
    CoWLibrary(bool Summary=true):m_libName("Summary"),m_summary(Summary){};
    CoWLibrary(const std::string &n,bool Summary=true):m_libName(n),m_ms(Summary),m_summary(Summary){};
    std::unordered_map<std::string,std::shared_ptr<CoWRecord> > m_records;
    void parseRecord(std::istream& in);
    std::string m_libName;
    CoWRecordStats m_ms;
    static std::shared_ptr<CoWLibrary> fromRecord(std::istream &in,bool summary);
  private:
    bool m_summary;
    friend std::ostream & operator<<(std::ostream &, const CoWLibrary &l);
  public:

    friend CoWLibrary operator-(CoWLibrary lhs, const CoWLibrary& rhs){
      if(lhs.m_libName!=rhs.m_libName){
	std::cerr<<"Warning trying to subtract different libraries. Ignoring"<<std::endl;
	return lhs;
      }
      return lhs-=rhs;
    }

    explicit operator bool() const {
      if(m_summary){
	auto vals=m_ms.getValueArray();
	for(int i=0;i<11;i++){
	  if(vals[i])return true;
	}
	return false;
      }else{
	for(auto i : m_records){
	  if(*(i.second))return true;
	}
	return false;
      }
    }

    friend CoWLibrary operator+(CoWLibrary lhs, const CoWLibrary& rhs){
      if(lhs.m_libName!=rhs.m_libName){
	std::cerr<<"Warning trying to add different libraries. Ignoring"<<std::endl;
	return lhs;
      }
      return lhs+=rhs;
    }

    CoWLibrary operator-()const{
      CoWLibrary m(*this);
      m.m_ms=-m_ms;
      return m;
    }

    inline CoWLibrary& operator+=(const CoWLibrary& rhs){
      if(m_libName!=rhs.m_libName){
	std::cerr<<"Warning trying to add different libraries. Ignoring"<<std::endl;
	return *this;
      }
      for(auto i:rhs.m_records){
	auto l=m_records.find(i.first);
	if(l!=m_records.end()){
	  *(l->second)+=*(i.second);
	}else{
	  m_records.insert(std::make_pair(i.first,std::make_shared<CoWRecord>(*(i.second))));
	}
      }
      m_ms+=rhs.m_ms;
      return *this;
    }

    inline CoWLibrary& operator-=(const CoWLibrary& rhs){
      if(m_libName!=rhs.m_libName){
	std::cerr<<"Warning trying to subtract different libraries. Ignoring"<<std::endl;
	return *this;
      }
      for(auto i:rhs.m_records){
	auto l=m_records.find(i.first);
	if(l!=m_records.end()){
	  *(l->second)-=*(i.second);
	}else{
	  m_records.insert(std::make_pair(i.first,std::make_shared<CoWRecord>(-*(i.second))));
	}
      }
      m_ms-=rhs.m_ms;
      return *this;
    }
  };

}//end namespace

#endif
