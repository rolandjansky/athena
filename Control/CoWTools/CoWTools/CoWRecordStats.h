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
	m_vals[i]+=rhs.m_vals[i];
      }
      m_vals[13]+=rhs.m_vals[13];
      //needed only for very detailed
      //m_vals[14]|=rhs.m_vals[14];
      return *this;
    }

    inline CoWRecordStats& operator-=(const CoWRecordStats& rhs){
      for(int i=0;i<11;i++){
	m_vals[i]-=rhs.m_vals[i];
      }
      m_vals[13]-=rhs.m_vals[13];
      //needed only for very detailed
      //m_vals[14]|=rhs.m_vals[14];
      return *this;
    }
    
    inline CoWRecordStats operator-()const{
      CoWRecordStats m(m_summary);
      for(int i=0;i<11;i++){
	m.m_vals[i]=-m_vals[i];
      }
      m.m_vals[11]=m_vals[11];
      m.m_vals[12]=m_vals[12];
      m.m_vals[13]=-m_vals[13];
      m.m_vals[14]=m_vals[14];
      //needed only for very detailed
      //m_vals[14]|=rhs.m_vals[14];
      return m;
    }
    friend CoWRecordStats operator+(const CoWRecordStats& lhs, const CoWRecordStats& rhs){
      return CoWRecordStats(lhs) += rhs;
    }
    friend CoWRecordStats operator-(const CoWRecordStats& lhs, const CoWRecordStats& rhs){
      return CoWRecordStats(lhs) -= rhs;
    }
    explicit operator bool() const {
      for(int i=0;i<11;i++){
	if(m_vals[i])return true;
      }
      return false;
    }
    friend bool operator<(const CoWRecordStats& a,const CoWRecordStats &b){
      if((a.m_vals[5]+a.m_vals[6])<(b.m_vals[5]+b.m_vals[6])){
	return true;
      }else if((a.m_vals[5]+a.m_vals[6])==(b.m_vals[5]+b.m_vals[6])){
	if((a.m_vals[3]+a.m_vals[4])<(b.m_vals[3]+b.m_vals[4])){
	  return true;
	}else if((a.m_vals[3]+a.m_vals[4])==(b.m_vals[3]+b.m_vals[4])){
	  return (a.m_vals[1]<b.m_vals[1]);
	}
	return false;
      }
      return false;
    }
    void parseRecord(std::istream &in);
    long long *  getValueArray(){return m_vals;}
    const long long *  getValueArray() const {return m_vals;}
  private:
    long long m_vals[20]={0,0,0,0,0,
                          0,0,0,0,0,
                          0,0,0,0,0,
                          0,0,0,0,0};

    bool m_summary;
    static const std::map<std::string,int> s_vmFlags;
    friend std::ostream & operator<<(std::ostream &, const CoWTools::CoWRecordStats &m);
  };

}//end namespace

#endif

