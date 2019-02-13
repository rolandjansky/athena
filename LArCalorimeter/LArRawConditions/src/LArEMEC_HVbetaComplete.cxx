/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArEMEC_HVbetaComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"


#include <iostream> 
using namespace std ;

float LArEMEC_HVbetaComplete::dummy = ERRORCODE;

LArEMEC_HVbetaComplete::LArEMEC_HVbetaComplete()  {}

LArEMEC_HVbetaComplete::~LArEMEC_HVbetaComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArEMEC_HVbetaComplete::set(const HWIdentifier& CellID, float EMEC_HVbeta ) {
  
  LArEMEC_HVbetaP  t; 
  t.m_EMEC_HVbeta = EMEC_HVbeta ; 
  
  setPdata(CellID, t); 

}

/* retrieve EMEC_HVbeta ******************************************************
 */
const float& LArEMEC_HVbetaComplete::EMEC_HVbeta(const HWIdentifier& CellID) const 
{ 
  const LArEMEC_HVbetaP& t = get( CellID); 
  
  return (t.m_EMEC_HVbeta);
}



  
