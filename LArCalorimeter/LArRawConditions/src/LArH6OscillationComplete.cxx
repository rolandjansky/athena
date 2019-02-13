/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArH6OscillationComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
using namespace std ;

LArH6OscillationComplete::LArH6OscillationComplete() {}

LArH6OscillationComplete::~LArH6OscillationComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArH6OscillationComplete::set(const HWIdentifier& CellID,
			      double channelPhase, double channelAmplitude ) {
  
  LArH6OscillationP t ; 
  t.m_channelPhase    = channelPhase;
  t.m_channelAmplitude = channelAmplitude;

  setPdata(CellID,t); 
}

/* retrieve channel phases and channel amplitudes*****************************
 */
const double& LArH6OscillationComplete::channelPhase(const HWIdentifier& CellID) const 
{ 
  
  const LArH6OscillationP& t = get(CellID); 

  return t.m_channelPhase;
}

const double& LArH6OscillationComplete::channelAmplitude(const HWIdentifier& CellID) const 
{ 
  
  const LArH6OscillationP& t = get(CellID); 

  return t.m_channelAmplitude;
}
