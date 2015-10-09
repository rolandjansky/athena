/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArRampComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArTools/LArCablingService.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
using namespace std ;

LArRampComplete::LArRampComplete():LArRampCompleteBase()   
{

}

LArRampComplete::~LArRampComplete() {

}


LArRampCompleteBase::LArRampCompleteBase()  
{

}

LArRampCompleteBase::~LArRampCompleteBase() {
}

/* Fill transient object in ATHENA *****************************************
void LArRampCompleteBase::setPersistent( LArRampP* d){
    if(m_db) delete m_db; 
    m_db = d; 
}

LArRampP* LArRampCompleteBase::getPersistent( ){
    return m_db; 
}
*/


void LArRampCompleteBase::set(const HWIdentifier& CellID, int gain,
			     const std::vector<float>& vRamp ) {
  
  LArRampP1 t;
  t.m_vRamp = vRamp; 
  setPdata(CellID,t, (unsigned int)gain); 

}


/* retrieve Ramp ******************************************************
 */
 LArRampCompleteBase::RampRef_t LArRampCompleteBase::ADC2DAC(const HWIdentifier& CellID, int gain) const 
{ 
  
  const LArRampP1& t = get(CellID,gain) ;   
  return ( t.m_vRamp );
}

 LArRampCompleteBase::RampRef_t LArRampCompleteBase::ADC2DAC(const Identifier&  CellID, int gain) const
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
      MsgStream logstr(Athena::getMessageSvc(), "LArRampComplete");
      logstr << MSG::WARNING << "Could not retrieve LArCablingService Tool " << endreq;
      return empty(); 
    }
    OnId = cablingService->createSignalChannelID(CellID);  
    
  } else {
    MsgStream logstr(Athena::getMessageSvc(), "LArRampComplete");
    logstr << MSG::WARNING << "Could not retrieve ToolSvc " << endreq;
    return empty(); 
  }

  return ADC2DAC(OnId, gain);
}

const std::vector<float>& LArRampCompleteBase::empty() const 
{
    static const std::vector<float> result;
    return (result);
}



     


  

