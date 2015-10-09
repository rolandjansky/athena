/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArMinBiasComplete.h" 

#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include <iostream> 
using namespace std ;

LArMinBiasComplete::LArMinBiasComplete()  {}

LArMinBiasComplete::~LArMinBiasComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArMinBiasComplete::set(const HWIdentifier& CellID, float minBiasRMS ) 
{  
  LArMinBiasP t ; 
  t.m_MinBiasRMS  = minBiasRMS;

  setPdata(CellID,t); 
}


/* retrieve RMS of E in minimum bias events **********************************
 */
const float& LArMinBiasComplete::minBiasRMS(const HWIdentifier& CellID) const 
{ 
  
  const LArMinBiasP& t = get(CellID); 
  
  return t.m_MinBiasRMS ;
}

const float& LArMinBiasComplete::minBiasRMS(const Identifier&  CellID) const
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
      std::cout<<"LArMinBiasComplete: "
	       <<"Could not retrieve LArCablingService Tool"
	       << std::endl;
      return dummy; 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    std::cout << "LArMinBiasComplete: Could not retrieve ToolSvc " 
	      << std::endl;
    return dummy; 
  }
  return minBiasRMS(OnId);
}


  

