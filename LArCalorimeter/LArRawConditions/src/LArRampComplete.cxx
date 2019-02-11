/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArRampComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
using namespace std ;

LArRampComplete::LArRampComplete():LArRampCompleteBase()   
{}

LArRampComplete::~LArRampComplete() {}


LArRampCompleteBase::LArRampCompleteBase() {}

LArRampCompleteBase::~LArRampCompleteBase() {}

void LArRampCompleteBase::set(const HWIdentifier& CellID, int gain,
			      const std::vector<float>& vRamp ) {
  
  LArRampP1 t;
  t.m_vRamp = vRamp; 
  setPdata(CellID,t, (unsigned int)gain); 
}


//retrieve Ramp 
 LArRampCompleteBase::RampRef_t LArRampCompleteBase::ADC2DAC(const HWIdentifier& CellID, int gain) const  {  
  const LArRampP1& t = get(CellID,gain) ;   
  return ( t.m_vRamp );
}

const std::vector<float>& LArRampCompleteBase::empty() const {
    static const std::vector<float> result;
    return (result);
}



     


  

