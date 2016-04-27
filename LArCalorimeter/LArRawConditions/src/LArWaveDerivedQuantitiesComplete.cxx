/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArWaveDerivedQuantitiesComplete.h" 

#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream>
using namespace std ;

LArWaveDerivedQuantitiesComplete::LArWaveDerivedQuantitiesComplete() {}
LArWaveDerivedQuantitiesComplete::~LArWaveDerivedQuantitiesComplete() {}

/* 
 * Fill transient object in ATHENA
 */

void LArWaveDerivedQuantitiesComplete::set(const HWIdentifier& CellID, int gain,
					   float baseline, float maxAmp, 
					   float tmaxAmp,  float width, 
					   float rT0,      float posLobe,
					   float jitter,   unsigned flag)
{
  
  LArWaveDerivedQuantitiesP t ; 
  
  t.m_baseline = baseline;
  t.m_maxAmp   = maxAmp;
  t.m_tmaxAmp  = tmaxAmp;
  t.m_width    = width;
  t.m_rT0      = rT0;
  t.m_posLobe  = posLobe;
  t.m_jitter  = jitter;
  t.m_flag     = flag;

  setPdata(CellID,t,gain) ;
}

/* 
 * retrieve Wave Derived Quantities
 */

const float& LArWaveDerivedQuantitiesComplete::BaseLine(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_baseline);
}

const float& LArWaveDerivedQuantitiesComplete::MaxAmp(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_maxAmp);
}

const float& LArWaveDerivedQuantitiesComplete::TMaxAmp(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_tmaxAmp);
}

const float& LArWaveDerivedQuantitiesComplete::Width(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_width);
}

const float& LArWaveDerivedQuantitiesComplete::rT0(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_rT0);
}

const float& LArWaveDerivedQuantitiesComplete::PosLobe(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_posLobe);
}

const float& LArWaveDerivedQuantitiesComplete::Jitter(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_jitter);
}


const unsigned& LArWaveDerivedQuantitiesComplete::Flag(const HWIdentifier& CellID, int gain) const 
{ 
  const LArWaveDerivedQuantitiesP& t = get(CellID,gain) ;
  return (t.m_flag);
}


const float& LArWaveDerivedQuantitiesComplete::BaseLine(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return BaseLine(OnId, gain);
}


const float& LArWaveDerivedQuantitiesComplete::MaxAmp(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return MaxAmp(OnId, gain);
}


const float& LArWaveDerivedQuantitiesComplete::TMaxAmp(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return TMaxAmp(OnId, gain);
}

const float& LArWaveDerivedQuantitiesComplete::Width(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return Width(OnId, gain);
}


const float& LArWaveDerivedQuantitiesComplete::rT0(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return rT0(OnId, gain);
}


const float& LArWaveDerivedQuantitiesComplete::PosLobe(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return PosLobe(OnId, gain);
}


const float& LArWaveDerivedQuantitiesComplete::Jitter(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return Jitter(OnId, gain);
}

const unsigned& LArWaveDerivedQuantitiesComplete::Flag(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static unsigned empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWaveDerivedQuantitiesComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static unsigned empty = ERRORCODE ; 
    return empty; 
  }
  return Flag(OnId, gain);
}
