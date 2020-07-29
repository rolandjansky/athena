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
NSWTrigOut::NSWTrigOut(int side, std::vector<int> NSWTP, std::vector<int> NSWeta, std::vector<int> NSWphi, std::vector<int> NSWDtheta, TGCArguments* tgcargs)
  : m_sideID(side), m_NSWTriggerProcessor(NSWTP), m_NSWeta_8bit(NSWeta), m_NSWphi_6bit(NSWphi), m_NSWDtheta_5bit(NSWDtheta), m_tgcArgs(tgcargs)
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
        << endmsg;
  }
  
}


} //end of namespace bracket
