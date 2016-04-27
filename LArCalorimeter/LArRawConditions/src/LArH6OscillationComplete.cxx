/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArH6OscillationComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
using namespace std ;

LArH6OscillationComplete::LArH6OscillationComplete() {}

LArH6OscillationComplete::~LArH6OscillationComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArH6OscillationComplete::set(const HWIdentifier& CellID,
			      double channelPhase, double channelAmplitude ) {
  
  LArH6OscillationP t ; 
  t.m_channelPhase    = channelPhase;
  t.m_channelAmplitude = channelAmplitude;

  setPdata(CellID,t); 
}

/* retrieve channel phases and channel amplitudes*****************************
 */
const double& LArH6OscillationComplete::channelPhase(const HWIdentifier& CellID) const 
{ 
  
  const LArH6OscillationP& t = get(CellID); 

  return t.m_channelPhase;
}

const double& LArH6OscillationComplete::channelAmplitude(const HWIdentifier& CellID) const 
{ 
  
  const LArH6OscillationP& t = get(CellID); 

  return t.m_channelAmplitude;
}

const double& LArH6OscillationComplete::channelPhase(const Identifier&  CellID) const
{
  static double empty(0.); 
  HWIdentifier OnId;

  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      MsgStream logstr(Athena::getMessageSvc(), "LArH6OscillationComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArH6OscillationComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    return empty; 
  }
  return channelPhase(OnId);
}

const double& LArH6OscillationComplete::channelAmplitude(const Identifier&  CellID) const
{
  HWIdentifier OnId;
  static double empty(0.); 

  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      MsgStream logstr(Athena::getMessageSvc(), "LArH6OscillationComplete");
      logstr << MSG::WARNING << "LArH6OscillationComplete: Could not retrieve LArCablingService Tool " << endreq;
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArH6OscillationComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    return empty; 
  }
  return channelAmplitude(OnId);
}


  





