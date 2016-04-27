/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArEMEC_CphiComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"


#include <iostream> 
using namespace std ;

float LArEMEC_CphiComplete::dummy = ERRORCODE;

LArEMEC_CphiComplete::LArEMEC_CphiComplete()  {}

LArEMEC_CphiComplete::~LArEMEC_CphiComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArEMEC_CphiComplete::set(const HWIdentifier& CellID, float EMEC_Cphi ) {
  
  LArEMEC_CphiP  t; 
  t.m_EMEC_Cphi = EMEC_Cphi ; 
  
  setPdata(CellID, t); 

}


/* retrieve EMEC_Cphi ******************************************************
 */
const float& LArEMEC_CphiComplete::EMEC_Cphi(const HWIdentifier& CellID) const 
{ 
  const LArEMEC_CphiP& t = get( CellID); 
  
  return (t.m_EMEC_Cphi);
}

const float& LArEMEC_CphiComplete::EMEC_Cphi(const Identifier&  CellID) const
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
      std::cout << "LArEMEC_Cphi: Could not retrieve LArCablingService Tool " << std::endl;
      return dummy; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    std::cout << "LArEMEC_Cphi: Could not retrieve ToolSvc " << std::endl;
    return dummy; 
  }

  return EMEC_Cphi(OnId);
}


  
