/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArEMEC_HValphaComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"


#include <iostream> 
using namespace std ;

float LArEMEC_HValphaComplete::dummy = ERRORCODE;

LArEMEC_HValphaComplete::LArEMEC_HValphaComplete()  {}

LArEMEC_HValphaComplete::~LArEMEC_HValphaComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArEMEC_HValphaComplete::set(const HWIdentifier& CellID, float EMEC_HValpha ) {
  
  LArEMEC_HValphaP  t; 
  t.m_EMEC_HValpha = EMEC_HValpha ; 
  
  setPdata(CellID, t); 

}

/* retrieve EMEC_HValpha ******************************************************
 */
const float& LArEMEC_HValphaComplete::EMEC_HValpha(const HWIdentifier& CellID) const 
{ 
  const LArEMEC_HValphaP& t = get( CellID); 
  
  return (t.m_EMEC_HValpha);
}

const float& LArEMEC_HValphaComplete::EMEC_HValpha(const Identifier&  CellID) const
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
      std::cout << "LArEMEC_HValpha: Could not retrieve LArCablingService Tool " << std::endl;
      return dummy; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    std::cout << "LArEMEC_HValpha: Could not retrieve ToolSvc " << std::endl;
    return dummy; 
  }

  return EMEC_HValpha(OnId);
}


  
