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

// ====================================================================
//
// class definition
//
// ====================================================================
 
class NSWTrigOut {
protected:
  int            m_sideID=-1;     // 0:A-side 1:C-side
  std::vector<int>            m_NSWTriggerProcessor;   // 0 ~ 15
  std::vector<int>            m_NSWeta_8bit;     //  0.005
  std::vector<int>            m_NSWphi_6bit;      // 10mrad
  std::vector<int>            m_NSWDtheta_5bit;    // 1mrad

public:
  NSWTrigOut();
  NSWTrigOut(int side, std::vector<int> NSWTrigger, std::vector<int> NSWeta, std::vector<int> NSWphi, std::vector<int> NSWDtheta); 
  NSWTrigOut(int side, std::vector<int> NSWTrigger); 

  virtual ~NSWTrigOut() { }
 
  NSWTrigOut(const NSWTrigOut& right);
 
  NSWTrigOut& operator=(const NSWTrigOut& right);
  NSWTrigOut& operator+(const NSWTrigOut& right);
  NSWTrigOut& operator+=(const NSWTrigOut& right);
  int operator==(const NSWTrigOut& right) const
  {
    return (this==&right);
  }
 
  int operator!=(const NSWTrigOut& right) const
  {
    return (this!=&right);
  }



  // set functons
  void SetSide(int side){ m_sideID = side; }
  void SetNSWTriggerProcessor(int NSWTP){ m_NSWTriggerProcessor.push_back(NSWTP); }
  void SetEta(int NSWeta){ m_NSWeta_8bit.push_back(NSWeta); }
  void SetPhi(int NSWphi){ m_NSWphi_6bit.push_back(NSWphi); }
  void SetDtheta(int NSWDtheta){ m_NSWDtheta_5bit.push_back(NSWDtheta); }
  void clear() { m_sideID=-1;m_NSWTriggerProcessor.clear(); m_NSWeta_8bit.clear();  m_NSWphi_6bit.clear(); m_NSWDtheta_5bit.clear();}

  // get functions
  int GetSide() const { return m_sideID; }
  std::vector<int> GetNSWTriggerProcessor() const {return m_NSWTriggerProcessor; }
  std::vector<int> GetNSWeta() const {return m_NSWeta_8bit; }
  std::vector<int> GetNSWphi() const {return m_NSWphi_6bit; }
  std::vector<int> GetNSWDtheta() const {return m_NSWDtheta_5bit; }

  // methods  
  void print() const;

};


} //end of namespace bracket

#endif
