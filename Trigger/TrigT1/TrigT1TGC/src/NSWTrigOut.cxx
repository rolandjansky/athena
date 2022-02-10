/*
  Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration
*/

// ====================================================================
/*
        NSWTrigOut.cc             
*/
// ====================================================================

#include "AthenaKernel/getMessageSvc.h"
#include "TrigT1TGC/NSWTrigOut.h"
#include "TrigT1TGC/TGCArguments.h"

namespace LVL1TGCTrigger {


///////////////////////////////////////////////////////////////
NSWTrigOut::NSWTrigOut(int side, std::vector<int> NSWTP, std::vector<uint8_t> NSWeta, std::vector<uint8_t> NSWphi, std::vector<uint8_t> NSWDtheta, std::vector<bool> NSWlowRes, std::vector<bool> NSWphiRes, std::vector<bool> NSWmonitor, TGCArguments* tgcargs)
  : m_sideID(side), m_NSWTriggerProcessor(NSWTP), m_NSWeta_8bit(NSWeta), m_NSWphi_6bit(NSWphi), m_NSWDtheta_5bit(NSWDtheta), m_NSWlowRes(NSWlowRes), m_NSWphiRes(NSWphiRes), m_NSWmonitor(NSWmonitor), m_tgcArgs(tgcargs)
///////////////////////////////////////////////////////////////
{
}

///////////////////////////////////////////////////////////////
NSWTrigOut::NSWTrigOut(int side, std::vector<int> NSWTP, TGCArguments* tgcargs)
  : m_sideID(side), m_NSWTriggerProcessor(NSWTP), m_tgcArgs(tgcargs)
///////////////////////////////////////////////////////////////
{
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
    m_NSWlowRes.insert(m_NSWlowRes.end(), right.m_NSWlowRes.begin(), right.m_NSWlowRes.end());
    m_NSWphiRes.insert(m_NSWphiRes.end(), right.m_NSWphiRes.begin(), right.m_NSWphiRes.end());
    m_NSWmonitor.insert(m_NSWmonitor.end(), right.m_NSWmonitor.begin(), right.m_NSWmonitor.end());
  }
  return *this;
}

/////////////////////////////
void NSWTrigOut::print() const
/////////////////////////////
{
  MsgStream msg(Athena::getMessageSvc(), "LVL1TGC::NSWTrigOut");
  if (m_tgcArgs) msg.setLevel(m_tgcArgs->MSGLEVEL());

  for(unsigned int i=0;i!=m_NSWTriggerProcessor.size();i++){
    msg << MSG::DEBUG
        << "i=" << i<<"\n"
        <<"Size=="<<m_NSWTriggerProcessor.size()<<"\n"
        << "Side=" << m_sideID <<"\n"
        << " :: ModuleID=" << m_NSWTriggerProcessor[i]<<"\n"
        << " :: eta_8bit=" << m_NSWeta_8bit[i]<<"\n"
        << " :: phi_6bit=" << m_NSWphi_6bit[i]<<"\n"
        << " :: Dtheta_5bit=" << m_NSWDtheta_5bit[i]<<"\n"
	<< " :: lowRes=" << m_NSWlowRes[i] <<"\n"
	<< " :: phiRes=" << m_NSWphiRes[i] <<"\n"
	<< " :: monitor=" << m_NSWmonitor[i] <<"\n"
        << endmsg;
  }
  
}


} //end of namespace bracket
