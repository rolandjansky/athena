/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArWFParamsComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
//using namespace std ;

LArWFParams LArWFParamsComplete::s_dummyParams = LArWFParams() ;

LArWFParamsComplete::LArWFParamsComplete()  
{

}

LArWFParamsComplete::~LArWFParamsComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArWFParamsComplete::set(const HWIdentifier& CellID, int gain, LArWFParams theParams) {

  setPdata(CellID, theParams, gain); 

}

/* retrieve WF params using online ID ****************************
 */
const LArWFParams& LArWFParamsComplete::getParams(const HWIdentifier& CellID, int gain) const 
{ 

  return get(CellID,gain); 

}

/* retrieve WF params using offline ID ****************************
 */
const LArWFParams& LArWFParamsComplete::getParams(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArOFCComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      //static std::vector<float> empty; 
      return s_dummyParams; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArWFParamsComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    //static std::vector<float> empty; 
    return s_dummyParams ; 
  }
  return get(OnId, gain);
} 

