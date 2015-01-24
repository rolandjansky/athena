// --*- c++ -*-- 

/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef COWTOOLS_RECORDSTATS
#define COWTOOLS_RECORDSTATS
#include <map>
#include <iostream>

namespace CoWTools{
  class CoWRecordStats{
  public:
    CoWRecordStats(bool summary=true):m_summary(summary){};
    enum ValueIdx{
      Size=0,
      Rss=1,
      Pss=2,
      Shared_Clean=3,
      Shared_Dirty=4,
      Private_Clean=5,
      Private_Dirty=6,
      Referenced=7,
      Anonymous=8,
      AnonHugePages=9,
      Swap=10,
      KernelPageSize=11,
      MMUPageSize=12,
      Locked=13,
      VmFlags=14
    };
  

    inline CoWRecordStats& operator+=(const CoWRecordStats& rhs){
      for(int i=0;i<11;i++){
	vals[i]+=rhs.vals[i];
      }
      vals[13]+=rhs.vals[13];
      //needed only for very detailed
      //vals[14]|=rhs.vals[14];
      return *this;
    }

    inline CoWRecordStats& operator-=(const CoWRecordStats& rhs){
      for(int i=0;i<11;i++){
	vals[i]-=rhs.vals[i];
      }
      vals[13]-=rhs.vals[13];
      //needed only for very detailed
      //vals[14]|=rhs.vals[14];
      return *this;
    }
    
    friend CoWRecordStats operator+(CoWRecordStats lhs, const CoWRecordStats& rhs){
      return lhs+=rhs;
    }
    friend CoWRecordStats operator-(CoWRecordStats lhs, const CoWRecordStats& rhs){
      return lhs-=rhs;
    }
    explicit operator bool() const {
      for(int i=0;i<11;i++){
	if(vals[i])return true;
      }
      return false;
    }
    friend bool operator<(const CoWRecordStats& a,const CoWRecordStats &b){
      if((a.vals[5]+a.vals[6])<(b.vals[5]+b.vals[6])){
	return true;
      }else if((a.vals[5]+a.vals[6])==(b.vals[5]+b.vals[6])){
	if((a.vals[3]+a.vals[4])<(b.vals[3]+b.vals[4])){
	  return true;
	}else if((a.vals[3]+a.vals[4])==(b.vals[3]+b.vals[4])){
	  return (a.vals[1]<b.vals[1]);
	}
	return false;
      }
      return false;
    }
    CoWRecordStats(const CoWRecordStats &ms){
      m_summary=ms.m_summary;
      for(int i=0;i<20;i++){
	vals[i]=ms.vals[i];
      }
    }
    void parseRecord(std::istream &in);
    long long *  getValueArray(){return vals;}
  private:
    long long vals[20]={0,0,0,0,0,
			0,0,0,0,0,
			0,0,0,0,0,
			0,0,0,0,0};

    bool m_summary;
    static const std::map<std::string,int> vmFlags;
    friend std::ostream & operator<<(std::ostream &, const CoWTools::CoWRecordStats &m);
  };

}//end namespace

#endif

