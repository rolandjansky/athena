/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArPhysCaliTdiffComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
using namespace std ;

// const float LArPhysCaliTdiffComplete::empty = ERRORCODE ;
LArPhysCaliTdiffComplete::LArPhysCaliTdiffComplete()  
{
}

LArPhysCaliTdiffComplete::~LArPhysCaliTdiffComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArPhysCaliTdiffComplete::set(const HWIdentifier& CellID, int gain, 
				     float tdiff ) {
  
  LArPhysCaliTdiffP t; 
  t.m_Tdiff        = tdiff ;

  setPdata(CellID,t,gain) ;
}

/* retrieve PhysCaliTdiff ******************************************************
 */
const float& LArPhysCaliTdiffComplete::Tdiff(const HWIdentifier& CellID, int gain) const 
{ 

  const LArPhysCaliTdiffP& t= get(CellID,gain); 

  return t.m_Tdiff;
}

const float& LArPhysCaliTdiffComplete::Tdiff(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArPhysCaliTdiffComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      static float empty = ERRORCODE ; 
      return empty; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArPhysCaliTdiffComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    static float empty = ERRORCODE ; 
    return empty; 
  }
  return Tdiff(OnId, gain);
}


  

