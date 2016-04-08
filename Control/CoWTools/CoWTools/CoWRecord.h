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
    CoWRecord(bool summary=true):
      addrStart(0),
      addrEnd(0),
      perms(0),
      offset(0),
      inode(0),
      m_ms(summary),
      dmin(0),
      dmaj(0),
      m_summary(summary)
    {}
    unsigned long addrStart,addrEnd;
    short perms;
    unsigned long long offset;
    unsigned long inode;
    CoWRecordStats m_ms;
    unsigned int dmin,dmaj;
    void parseRecord(std::istream& in);    
  private:
    bool m_summary;
  public:
    friend CoWRecord operator-(CoWRecord lhs, const CoWRecord& rhs){
      return lhs-=rhs;
    }
    CoWRecord operator-()const{
      CoWRecord m(*this);
      m.m_ms=-m_ms;
      return m;
    }
    explicit operator bool() const {
      auto vals=m_ms.getValueArray();
      for(int i=0;i<11;i++){
	if(vals[i])return true;
      }
      return false;
    }

    friend CoWRecord operator+(CoWRecord lhs, const CoWRecord& rhs){
      return lhs+=rhs;
    }

    inline CoWRecord& operator+=(const CoWRecord& rhs){
      perms|=rhs.perms;
      m_ms+=rhs.m_ms;
      return *this;
    }

    inline CoWRecord& operator-=(const CoWRecord& rhs){
      perms^=rhs.perms;
      m_ms-=rhs.m_ms;
      return *this;
    }
  };

}//end namespace

#endif
