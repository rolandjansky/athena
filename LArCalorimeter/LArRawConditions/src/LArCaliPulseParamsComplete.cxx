/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArCaliPulseParamsComplete.h" 

#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
using namespace std ;

LArCaliPulseParamsComplete::LArCaliPulseParamsComplete()  
{}

LArCaliPulseParamsComplete::~LArCaliPulseParamsComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArCaliPulseParamsComplete::set(const HWIdentifier& CellID, int gain, 
				     float tcal, float fstep ) {
  
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;

  setPdata(CellID,t,gain) ;
}
void LArCaliPulseParamsComplete::set(const HWIdentifier& CellID, int gain, 
				     float tcal, float fstep, float offset, float dtcal ) {
  
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;
  t.m_Offset       = offset ;
  t.m_dTimeCal     = dtcal ;

  setPdata(CellID,t,gain) ;
}
void LArCaliPulseParamsComplete::set(const HWIdentifier& CellID, int gain, 
				     float tcal, float fstep, float offset, float dtcal, short ncb ) {
  
  LArCaliPulseParamsP t ; 
  t.m_Tcal         = tcal ;
  t.m_Fstep        = fstep ;
  t.m_Offset       = offset ;
  t.m_dTimeCal     = dtcal ;
  t.m_nCB          = ncb ;

  setPdata(CellID,t,gain) ;
}


/* retrieve CaliPulseParams ******************************************************
 */
const float& LArCaliPulseParamsComplete::Tcal(const HWIdentifier& CellID, int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_Tcal);
}

const float& LArCaliPulseParamsComplete::Fstep(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_Fstep);
}

const float& LArCaliPulseParamsComplete::Offset(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_Offset);
}

const float& LArCaliPulseParamsComplete::dTimeCal(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_dTimeCal);
}

const short& LArCaliPulseParamsComplete::nCB(const HWIdentifier& CellID,int gain) const 
{ 
  const LArCaliPulseParamsP& t = get(CellID,gain) ;
  return (t.m_nCB);
}

const float& LArCaliPulseParamsComplete::Tcal(const Identifier&  CellID, int gain) const
{
  HWIdentifier OnId;
  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return Tcal(OnId, gain);
}

const float& LArCaliPulseParamsComplete::Fstep(const Identifier&  CellID, int gain) const
{
  HWIdentifier OnId;
  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
      logstr << MSG::WARNING << "LArCaliPulseParamsComplete: Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return Fstep(OnId, gain);
}

const float& LArCaliPulseParamsComplete::Offset(const Identifier&  CellID, int gain) const
{
  HWIdentifier OnId;
  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
      logstr << MSG::WARNING << "LArCaliPulseParamsComplete: Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return Offset(OnId, gain);
}

const float& LArCaliPulseParamsComplete::dTimeCal(const Identifier&  CellID, int gain) const
{
  HWIdentifier OnId;
  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
      logstr << MSG::WARNING << "LArCaliPulseParamsComplete: Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return dTimeCal(OnId, gain);
}

const short& LArCaliPulseParamsComplete::nCB(const Identifier&  CellID, int gain) const
{
  HWIdentifier OnId;
  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
      logstr << MSG::WARNING << "LArCaliPulseParamsComplete: Could not retrieve LArCablingService Tool " << endreq;
      static short empty = -999 ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArCaliPulseParamsComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static short empty = -999 ; 
    return empty; 
  }
  return nCB(OnId, gain);
}


  

