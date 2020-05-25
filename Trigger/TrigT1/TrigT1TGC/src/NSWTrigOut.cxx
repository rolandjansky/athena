/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        NSWTrigOut.cc             
*/
// ====================================================================

#include <iostream>
#include <iomanip>

#include "TrigT1TGC/NSWTrigOut.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class description
//
// ====================================================================

//////////////////////
NSWTrigOut::NSWTrigOut()
  : m_sideID(-1), m_NSWTriggerProcessor(0), m_NSWeta_8bit(0), m_NSWphi_6bit(0), m_NSWDtheta_5bit(0)
//////////////////////
{
}

///////////////////////////////////////////////////////////////
  NSWTrigOut::NSWTrigOut(int side, std::vector<int> NSWTP, std::vector<int> NSWeta,std::vector<int> NSWphi, std::vector<int> NSWDtheta)
    : m_sideID(side), m_NSWTriggerProcessor(NSWTP), m_NSWeta_8bit(NSWeta), m_NSWphi_6bit(NSWphi), m_NSWDtheta_5bit(NSWDtheta)
///////////////////////////////////////////////////////////////
{
}
  NSWTrigOut::NSWTrigOut(int side, std::vector<int> NSWTP)
    : m_sideID(side), m_NSWTriggerProcessor(NSWTP), m_NSWeta_8bit(0), m_NSWphi_6bit(0), m_NSWDtheta_5bit(0)
///////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////
NSWTrigOut::NSWTrigOut(const NSWTrigOut& right)
/////////////////////////////////////////////////////////////
    :m_sideID(right.m_sideID), 
     m_NSWTriggerProcessor(right.m_NSWTriggerProcessor),
     m_NSWeta_8bit(right.m_NSWeta_8bit),
     m_NSWphi_6bit(right.m_NSWphi_6bit),
     m_NSWDtheta_5bit(right.m_NSWDtheta_5bit)
{
}


/////////////////////////////////////////////////////////////
const NSWTrigOut& NSWTrigOut::operator=(const NSWTrigOut& right)
/////////////////////////////////////////////////////////////
{
  if (this != &right) {
    m_sideID    = right.m_sideID; 
    m_NSWTriggerProcessor  = right.m_NSWTriggerProcessor;
    m_NSWeta_8bit    = right.m_NSWeta_8bit;
    m_NSWphi_6bit     = right.m_NSWphi_6bit;
    m_NSWDtheta_5bit     = right.m_NSWDtheta_5bit;
  }
  return *this;
}
 
///////////////////////////////////////////////////////////
NSWTrigOut& NSWTrigOut::operator+=(const NSWTrigOut& right)
///////////////////////////////////////////////////////////
{
  if (this != &right && (m_sideID==right.m_sideID || m_sideID==-1)) {
   m_NSWTriggerProcessor.insert(m_NSWTriggerProcessor.end(), right.m_NSWTriggerProcessor.begin(), right.m_NSWTriggerProcessor.end());
   m_NSWeta_8bit.insert(m_NSWeta_8bit.end(), right.m_NSWeta_8bit.begin(), right.m_NSWeta_8bit.end());
   m_NSWphi_6bit.insert(m_NSWphi_6bit.end(), right.m_NSWphi_6bit.begin(), right.m_NSWphi_6bit.end());
   m_NSWDtheta_5bit.insert(m_NSWDtheta_5bit.end(), right.m_NSWDtheta_5bit.begin(), right.m_NSWDtheta_5bit.end());
 }

 return *this;

}

/////////////////////////////
void NSWTrigOut::print() const 
/////////////////////////////
{
  
  for(unsigned int i=0;i!=m_NSWTriggerProcessor.size();i++){
    std::cout<<"i="<<i<<std::endl;
    std::cout<<"Size=="<<m_NSWTriggerProcessor.size()<<std::endl;
    std::cout << "Side=" << m_sideID <<std::endl;
    std::cout<< " :: ModuleID=" << m_NSWTriggerProcessor[i]<<std::endl;
    std::cout<< " :: eta_8bit=" << m_NSWeta_8bit[i]<<std::endl;
    std::cout<< " :: phi_6bit=" << m_NSWphi_6bit[i]<<std::endl;
    std::cout<< " :: Dtheta_5bit=" << m_NSWDtheta_5bit[i]<<std::endl;
	  
  }
  
}


} //end of namespace bracket
