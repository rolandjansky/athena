/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArCalibParams.h"
#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "StoreGate/StoreGateSvc.h"
#include "AthenaKernel/getMessageSvc.h"
#include "LArIdentifier/LArOnlineID.h"

LArCalibParams::LArCalibParams():
  m_onlineHelper(0),  
  m_uniqueConfig(false)
{} 

StatusCode LArCalibParams::initialize() 
{
  //std::cout << "Initialize of LArCalibParams" << std::endl;
  //Get online Helper via DetectorStore
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  StoreGateSvc* detStore;
  MsgStream log(Athena::getMessageSvc(), "LArCalibParams");
  StatusCode sc = svcLoc->service( "DetectorStore",detStore);
  if (sc.isSuccess()) {
    sc=detStore->retrieve(m_onlineHelper, "LArOnlineID");
    if (sc.isFailure()) {
      log << MSG::ERROR << "LArCalibParams: Can't get LArOnlineID" << endmsg;
      return (StatusCode::FAILURE);
    }
  }
  else {
      log << MSG::ERROR << "LArCalibParams: Can't get DetectorStore" << endmsg;
      return (StatusCode::FAILURE);
  }
  
  return (StatusCode::SUCCESS);

}  



LArCalibParams::~LArCalibParams()
{
  //std::cout << "Destructor of LArCalibParams" << std::endl;
}

void LArCalibParams::set(const HWIdentifier CalibModuleID, const unsigned nTrigger,
			 const  std::vector<unsigned>& Pattern, const std::vector<unsigned>& DAC, const std::vector<unsigned>& Delay)
  
{if (m_uniqueConfig) //Had unique configuration before
  {m_mCalibBoards.clear(); //Clear parameters
   m_uniqueConfig=false;
  }
 m_mCalibBoards[CalibModuleID].m_nTrigger=nTrigger;
 for (unsigned i=0;i<DAC.size();i++)
   m_mCalibBoards[CalibModuleID].m_DAC.push_back(DAC[i]);
 for (unsigned i=0;i<Delay.size();i++)
   m_mCalibBoards[CalibModuleID].m_Delay.push_back(Delay[i]);
 for (unsigned i=0;i<Pattern.size();i++) {
   m_mCalibBoards[CalibModuleID].m_Pattern.push_back(Pattern[i]);
 }
 return;
}


void LArCalibParams::set(const unsigned nTrigger, const std::vector<unsigned>& Pattern, 
			 const std::vector<unsigned>& DAC, const std::vector<unsigned>& Delay)
{// Assume only one set of parameters, valid for all boards.
 m_mCalibBoards.clear();
 m_uniqueConfig=true;
 const HWIdentifier CalibModuleID(0);
 m_mCalibBoards[CalibModuleID].m_nTrigger=nTrigger;
 for (unsigned i=0;i<DAC.size();i++)
   m_mCalibBoards[CalibModuleID].m_DAC.push_back(DAC[i]);
 for (unsigned i=0;i<Delay.size();i++)
   m_mCalibBoards[CalibModuleID].m_Delay.push_back(Delay[i]);
 for (unsigned i=0;i<Pattern.size();i++)
   m_mCalibBoards[CalibModuleID].m_Pattern.push_back(Pattern[i]);
 return;
}


/* We assume that calib-board configuration is set in the following order:
1) Trigger
2) Pattern
3) Delay
4) DAC

*/

bool LArCalibParams::CalibBoard::isPulsed(const unsigned event,  const unsigned short lineNumber) const
{ //The pattern is changed after nTrigger events.
  const unsigned PatternStepSize=m_nTrigger;
  unsigned iPattern=(event/PatternStepSize)%(m_Pattern.size()/4);
  iPattern*=4; //A pattern consists of 4 words (4*32bit= 128 bit)
  const unsigned nWord=lineNumber>>5;  //lineNumber/32;
  const unsigned Pattern=m_Pattern[iPattern+nWord];
  const unsigned nBit=lineNumber&0x1f; //lineNumber%32;
  return Pattern>>nBit & 0x1;
}
 

unsigned LArCalibParams::CalibBoard::Delay(const unsigned event) const
{ //The delay value is changed after nTrigger*nPattern events
  const unsigned DelayStepSize=m_nTrigger*m_Pattern.size()/4; //1 pattern = 4 words
  const unsigned iDelay=(event/DelayStepSize)%m_Delay.size();
  return m_Delay[iDelay];
}


unsigned LArCalibParams::CalibBoard::DAC(const unsigned event) const
{ //The DAC is changed after nTrigger*nDelay*nPattern events
  const unsigned DACStepSize=m_nTrigger*m_Delay.size()*m_Pattern.size()/4; //1 pattern = 4 words
  const unsigned iDAC=event/DACStepSize; 
  if (iDAC<m_DAC.size()) {
    //std::cout << "CalibBoard Returning DAC=" << m_DAC[iDAC] << " Index=" << iDAC << " Stepsize=" << DACStepSize << std::endl;
    return m_DAC[iDAC];
  }
  else  {
     MsgStream log(Athena::getMessageSvc(), "LArCalibParams::DAC");
     log << MSG::ERROR << "Event #" << event << ": DAC index out of range! (" 
	 << iDAC << "max=" << m_DAC.size() << "). Return kmaxDAC" << endmsg;
     return kmaxDAC;
  }
}



unsigned LArCalibParams::Delay(const unsigned event, const HWIdentifier calibLineID) const
{if (calibLineID.get_compact()==0) return 0; //Disconnected channel
 if (m_uniqueConfig &&  m_mCalibBoards.size()==1)
   return m_mCalibBoards.begin()->second.Delay(event);
 else {
   if (!m_onlineHelper) {
     MsgStream log(Athena::getMessageSvc(), "LArCalibParams::Delay");
     log << MSG::ERROR << "LArOnlineID not defined! Can't determine Delay value." << endmsg;
     return 0; 
   }
   const HWIdentifier calibModuleID=m_onlineHelper->calib_module_Id(calibLineID);
   const std::map<HWIdentifier, CalibBoard>::const_iterator map_it=m_mCalibBoards.find(calibModuleID);
   if (map_it==m_mCalibBoards.end())
     return 0; //Can't find Board configuration -> Assume board is not configured. 
    else
      return map_it->second.Delay(event);
   }
}

unsigned LArCalibParams::DAC(const unsigned event, const HWIdentifier calibLineID) const
{if (calibLineID.get_compact()==0) return 0; //Disconnected channel
 if (m_uniqueConfig &&  m_mCalibBoards.size()==1)
   return m_mCalibBoards.begin()->second.DAC(event);
 else {
   if (!m_onlineHelper) {
     MsgStream log(Athena::getMessageSvc(), "LArCalibParams::DAC");
     log << MSG::ERROR << "LArOnlineID not defined! Can't determine DAC value." << endmsg;
     return 0; 
   }
   const HWIdentifier calibModuleID=m_onlineHelper->calib_module_Id(calibLineID);
   const std::map<HWIdentifier, CalibBoard>::const_iterator map_it=m_mCalibBoards.find(calibModuleID);
   if (map_it==m_mCalibBoards.end())
     return 0; //Can't find Board configuration -> Assume board is not configured. 
   else
     return map_it->second.DAC(event);
 }
}


bool LArCalibParams::isPulsed(const unsigned event, const HWIdentifier calibLineID) const
{if (calibLineID.get_compact()==0) return false; //Disconnected channel
 if (!m_onlineHelper) {
   MsgStream log(Athena::getMessageSvc(), "LArCalibParams::isPulsed");
   log << MSG::ERROR << "LArOnlineID not defined! Can't determine isPulsed value." << endmsg;
   return false; 
 }
 const int line=m_onlineHelper->channel(calibLineID);
 if (m_uniqueConfig &&  m_mCalibBoards.size()==1) 
    return m_mCalibBoards.begin()->second.isPulsed(event, line);
 else
   {const HWIdentifier calibModuleID=m_onlineHelper->calib_module_Id(calibLineID);
    const std::map<HWIdentifier, CalibBoard>::const_iterator map_it=m_mCalibBoards.find(calibModuleID);
    if (map_it==m_mCalibBoards.end()) {
      return false;//Can't find Board configuration -> Assume board is not configured.
    }
    else
      return map_it->second.isPulsed(event,line);
   }
}


unsigned LArCalibParams::getNumberConfig(const HWIdentifier calibModuleID) const
{ //Number of configurations is nDAC*nDelay*nPattern
  if (m_uniqueConfig &&  m_mCalibBoards.size()==1) {
    const unsigned nConfig=m_mCalibBoards.begin()->second.m_DAC.size()*
      m_mCalibBoards.begin()->second.m_Delay.size()*m_mCalibBoards.begin()->second.m_Pattern.size()/4;
    return nConfig;
  }
  else {
    const std::map<HWIdentifier, CalibBoard>::const_iterator map_it=m_mCalibBoards.find(calibModuleID);
    if (map_it==m_mCalibBoards.end()) 
      return 1; //Can't find Board configuration -> Assume board is not configured.(always the same values) 
    else {
      const unsigned nConfig=map_it->second.m_DAC.size()*
	map_it->second.m_Delay.size()*map_it->second.m_Pattern.size()/4;
      return nConfig;
    }
  }
}



unsigned LArCalibParams::NTrigger(const HWIdentifier calibModuleID) const
{
 if (m_uniqueConfig &&  m_mCalibBoards.size()==1) 
   return m_mCalibBoards.begin()->second.m_nTrigger;
 else {
   const std::map<HWIdentifier, CalibBoard>::const_iterator map_it=m_mCalibBoards.find(calibModuleID);
   if (map_it==m_mCalibBoards.end())
     return 0xFFFFFFFF; //Can't find Board configuration -> Assume board is not configured.(nothing ever changes) 
   else
     return map_it->second.m_nTrigger;
   }
}
/*
const CalibBoard& LArCalibParams::getCalibBoardParams(const HWIdentifier calibModuleID) const 
{
  if (m_uniqueConfig &&  m_mCalibBoards.size()==1)
    return m_mCalibBoards.begin()->second;
  else
    return m_mCalibBoards[calibModuleID]; //Find would be better....
}
*/
