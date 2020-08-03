/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        NSWTrigOut.h
*/
// ====================================================================
#ifndef NSW_TRIGOUT_H
#define NSW_TRIGOUT_H

#include <vector>

namespace LVL1TGCTrigger {

  class TGCArguments;

// ====================================================================
//
// class definition
//
// ====================================================================
 
class NSWTrigOut {
protected:
  int                         m_sideID{-1};     // 0:A-side 1:C-side
  std::vector<int>            m_NSWTriggerProcessor;   // 0 ~ 15
  std::vector<int>            m_NSWeta_8bit;     //  0.005
  std::vector<int>            m_NSWphi_6bit;      // 10mrad
  std::vector<int>            m_NSWDtheta_5bit;    // 1mrad

public:
  NSWTrigOut() = default;
  NSWTrigOut(int side, std::vector<int> NSWTrigger, std::vector<int> NSWeta, std::vector<int> NSWphi, std::vector<int> NSWDtheta,TGCArguments* tgcargs=nullptr);
  NSWTrigOut(int side, std::vector<int> NSWTrigger,TGCArguments* tgcargs=nullptr);

  NSWTrigOut& operator+=(const NSWTrigOut& right);
  bool operator==(const NSWTrigOut& right) const
  {
    return (this==&right);
  }
 
  bool operator!=(const NSWTrigOut& right) const
  {
    return (this!=&right);
  }


  // set functons
  void setSide(int side){ m_sideID = side; }
  void setNSWTriggerProcessor(int NSWTP){ m_NSWTriggerProcessor.push_back(NSWTP); }
  void setEta(int NSWeta){ m_NSWeta_8bit.push_back(NSWeta); }
  void setPhi(int NSWphi){ m_NSWphi_6bit.push_back(NSWphi); }
  void setDtheta(int NSWDtheta){ m_NSWDtheta_5bit.push_back(NSWDtheta); }
  void clear() { m_sideID=-1;m_NSWTriggerProcessor.clear(); m_NSWeta_8bit.clear();  m_NSWphi_6bit.clear(); m_NSWDtheta_5bit.clear();}

  // get functions
  int getSide() const { return m_sideID; }
  const std::vector<int>& getNSWTriggerProcessor() const {return m_NSWTriggerProcessor; }
  const std::vector<int>& getNSWeta() const {return m_NSWeta_8bit; }
  const std::vector<int>& getNSWphi() const {return m_NSWphi_6bit; }
  const std::vector<int>& getNSWDtheta() const {return m_NSWDtheta_5bit; }

  // print methods 
  void print() const;

 private:
  TGCArguments* m_tgcArgs{nullptr};
};

} //end of namespace bracket

#endif
