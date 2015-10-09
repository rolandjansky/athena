/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasAverageComplete.h" 

#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include <iostream> 
using namespace std ;

LArMinBiasAverageComplete::LArMinBiasAverageComplete()  {}

LArMinBiasAverageComplete::~LArMinBiasAverageComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArMinBiasAverageComplete::set(const HWIdentifier& CellID, float minBiasAverage ) 
{  
  LArMinBiasAverageP t ; 
  t.m_MinBiasAverage  = minBiasAverage;

  setPdata(CellID,t); 
}

/* retrieve Average of E in minimum bias events **********************************
 */
const float& LArMinBiasAverageComplete::minBiasAverage(const HWIdentifier& CellID) const 
{ 
  
  const LArMinBiasAverageP& t = get(CellID); 
  
  return t.m_MinBiasAverage ;
}

const float& LArMinBiasAverageComplete::minBiasAverage(const Identifier&  CellID) const
{
  HWIdentifier OnId;
  static float dummy = ERRORCODE;

  // translate offline ID into online ID
  ISvcLocator* svcLoc = Gaudi::svcLocator( );
  IToolSvc* toolSvc;
  StatusCode sc = svcLoc->service( "ToolSvc",toolSvc  );
  if(sc.isSuccess()) {
    LArCablingService* cablingService;
    sc = toolSvc->retrieveTool("LArCablingService",cablingService);
    if(sc.isFailure()){
      std::cout<<"LArMinBiasAverageComplete: "
	       <<"Could not retrieve LArCablingService Tool"
	       << std::endl;
      return dummy; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    std::cout << "LArMinBiasAverageComplete: Could not retrieve ToolSvc " 
	      << std::endl;
    return dummy; 
  }
  return minBiasAverage(OnId);
}


  

