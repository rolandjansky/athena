/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        NSWTrigOut.cc             
*/
// ====================================================================

#include "TrigT1TGC/NSWTrigOut.h"

#include "TrigT1TGC/TGCArguments.hh"
#include "AthenaBaseComps/AthMsgStreamMacros.h"

namespace LVL1TGCTrigger {

// ====================================================================
//
// class description
//
// ====================================================================

//////////////////////
NSWTrigOut::NSWTrigOut()
  : m_sideID(-1), m_NSWTriggerProcessor(0), m_NSWeta_8bit(0), m_NSWphi_6bit(0), m_NSWDtheta_5bit(0), m_tgcArgs(0)
//////////////////////
{
}

///////////////////////////////////////////////////////////////
  NSWTrigOut::NSWTrigOut(int side, std::vector<int> NSWTP, std::vector<int> NSWeta,std::vector<int> NSWphi, std::vector<int> NSWDtheta,TGCArguments* tgcargs)
    : m_sideID(side), m_NSWTriggerProcessor(NSWTP), m_NSWeta_8bit(NSWeta), m_NSWphi_6bit(NSWphi), m_NSWDtheta_5bit(NSWDtheta),m_tgcArgs(tgcargs)
///////////////////////////////////////////////////////////////
{
  m_msg = Athena::MsgStreamMember("LVL1TGC::NSWTrigOut");
  if(m_tgcArgs){
    m_msg.get().setLevel(m_tgcArgs->MSGLEVEL());
  }
  else{
    m_msg.get().setLevel(defaultMSGLvl);
  }
}
  NSWTrigOut::NSWTrigOut(int side, std::vector<int> NSWTP,TGCArguments* tgcargs)
    : m_sideID(side), m_NSWTriggerProcessor(NSWTP), m_NSWeta_8bit(0), m_NSWphi_6bit(0), m_NSWDtheta_5bit(0),m_tgcArgs(tgcargs)
///////////////////////////////////////////////////////////////
{
  m_msg = Athena::MsgStreamMember("LVL1TGC::NSWTrigOut");
  if(m_tgcArgs){
    m_msg.get().setLevel(m_tgcArgs->MSGLEVEL());
  }
  else{
    m_msg.get().setLevel(defaultMSGLvl);
  }
}

///////////////////////////////////////////////////////////////
NSWTrigOut::NSWTrigOut(const NSWTrigOut& right)
/////////////////////////////////////////////////////////////
    :m_sideID(right.m_sideID), 
     m_NSWTriggerProcessor(right.m_NSWTriggerProcessor),
     m_NSWeta_8bit(right.m_NSWeta_8bit),
     m_NSWphi_6bit(right.m_NSWphi_6bit),
     m_NSWDtheta_5bit(right.m_NSWDtheta_5bit),
     m_tgcArgs(right.m_tgcArgs)
{
  m_msg = Athena::MsgStreamMember("LVL1TGC::NSWTrigOut");
  if(m_tgcArgs){
    m_msg.get().setLevel(m_tgcArgs->MSGLEVEL());
  }
  else{
    m_msg.get().setLevel(defaultMSGLvl);
  }
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
    m_tgcArgs    =  right.m_tgcArgs; 
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
    ATH_MSG_DEBUG("i="<<i<<"\n"
            <<"Size=="<<m_NSWTriggerProcessor.size()<<"\n"
            << "Side=" << m_sideID <<"\n"
            << " :: ModuleID=" << m_NSWTriggerProcessor[i]<<"\n"
            << " :: eta_8bit=" << m_NSWeta_8bit[i]<<"\n"
            << " :: phi_6bit=" << m_NSWphi_6bit[i]<<"\n"
            << " :: Dtheta_5bit=" << m_NSWDtheta_5bit[i]<<"\n");     
  }
  
}


} //end of namespace bracket
