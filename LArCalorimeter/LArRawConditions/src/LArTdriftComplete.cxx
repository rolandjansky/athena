/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArTdriftComplete.h" 
#include "AthenaKernel/getMessageSvc.h"
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include <iostream> 
using namespace std ;

LArTdriftComplete::LArTdriftComplete()  {}

LArTdriftComplete::~LArTdriftComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArTdriftComplete::set(const HWIdentifier& CellID, float tdrift ) {

  LArTdriftP t;   
  t.m_Tdrift       = tdrift ;

  setPdata(CellID,t); 

}


/* retrieve Tdrift ******************************************************
 */
const float& LArTdriftComplete::Tdrift(const HWIdentifier& CellID) const 
{ 
  const LArTdriftP& t = get(CellID); 
  return (t.m_Tdrift);
}

const float& LArTdriftComplete::Tdrift(const Identifier&  CellID) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArTdriftComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArTdriftComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return Tdrift(OnId);
}


  

