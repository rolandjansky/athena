/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef HWMAP_H
#define HWMAP_H
#include <vector>
#include <string>
#include "AthenaKernel/CLASS_DEF.h"
#include "AthenaKernel/CondCont.h"
namespace TRTCond{
  class HWMap{
  public:
    HWMap() {
    // Initialize HV-line/pad maps
    const int nBarrelPadsTotal = 32*(42+65+100);
    m_Barrel_HV_CoolChanNames = new std::vector<std::string>(nBarrelPadsTotal,"");
    m_Barrel_HV_CoolChanNums  = new std::vector<int>(nBarrelPadsTotal,-1);
    const int nEndcapCellsTotal = 32*3*(6*4+8*2);
    m_EndcapA_HV_CoolChanNames = new std::vector<std::string>(nEndcapCellsTotal,"");
    m_EndcapA_HV_CoolChanNums  = new std::vector<int>(nEndcapCellsTotal,-1);
    m_EndcapC_HV_CoolChanNames = new std::vector<std::string>(nEndcapCellsTotal,"");
    m_EndcapC_HV_CoolChanNums  = new std::vector<int>(nEndcapCellsTotal,-1);
    // These are moved to CondStore which takes care of their deletion 
  }

  virtual ~HWMap() {}

  const std::vector<std::string>* get_Barrel_HV_Names() const {return m_Barrel_HV_CoolChanNames; } 
  const std::vector<std::string>* get_EndcapA_HV_Names() const {return m_EndcapA_HV_CoolChanNames; } 
  const std::vector<std::string>* get_EndcapC_HV_Names() const {return m_EndcapC_HV_CoolChanNames; } 
  const std::vector<int>* get_Barrel_HV_Nums() const {return m_Barrel_HV_CoolChanNums; }
  const std::vector<int>* get_EndcapA_HV_Nums() const {return m_EndcapA_HV_CoolChanNums; }
  const std::vector<int>* get_EndcapC_HV_Nums() const {return m_EndcapC_HV_CoolChanNums; }
 
  void setBarrelName(int i, const std::string & name) {
    m_Barrel_HV_CoolChanNames->at(i)=name;
  }
  void setEndcapAName(int i, const std::string & name) {
    m_EndcapA_HV_CoolChanNames->at(i)=name;
  }
  void setEndcapCName(int i, const std::string & name) {
    m_EndcapC_HV_CoolChanNames->at(i)=name;
  }
 
 void setBarrelNum(int i, const int & channel_number) {
    m_Barrel_HV_CoolChanNums->at(i)=channel_number;
  }

 void setEndcapANum(int i, const int & channel_number) {
    m_EndcapA_HV_CoolChanNums->at(i)=channel_number;
  }

 void setEndcapCNum(int i, const int & channel_number) {
    m_EndcapC_HV_CoolChanNums->at(i)=channel_number;
  }
     
  private:

  std::vector<std::string>* m_Barrel_HV_CoolChanNames;
  std::vector<std::string>* m_EndcapA_HV_CoolChanNames;
  std::vector<std::string>* m_EndcapC_HV_CoolChanNames;
  std::vector<int>* m_Barrel_HV_CoolChanNums;
  std::vector<int>* m_EndcapA_HV_CoolChanNums;
  std::vector<int>* m_EndcapC_HV_CoolChanNums;

  };
}
CLASS_DEF(TRTCond::HWMap,132179951,1)
CONDCONT_DEF(TRTCond::HWMap,183899721);
#endif
