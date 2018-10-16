/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArWFParamsComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"
#include "LArCabling/LArCablingService.h"

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
const LArWFParams& LArWFParamsComplete::getParams(const HWIdentifier& CellID, int gain) const { 
  return get(CellID,gain); 
}
