/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#include "LArRawConditions/LArPhysCaliTdiffComplete.h" 
#include "GaudiKernel/IMessageSvc.h"

#include "LArIdentifier/LArOnlineID.h"

#include "GaudiKernel/Bootstrap.h"
#include "GaudiKernel/ISvcLocator.h"
#include "GaudiKernel/IToolSvc.h"

#include "AthenaKernel/getMessageSvc.h"
#include <iostream> 
using namespace std ;

// const float LArPhysCaliTdiffComplete::empty = ERRORCODE ;
LArPhysCaliTdiffComplete::LArPhysCaliTdiffComplete()  
{
}

LArPhysCaliTdiffComplete::~LArPhysCaliTdiffComplete() {}

/* Fill transient object in ATHENA *****************************************
 */
void LArPhysCaliTdiffComplete::set(const HWIdentifier& CellID, int gain, 
				     float tdiff ) {
  
  LArPhysCaliTdiffP t; 
  t.m_Tdiff        = tdiff ;

  setPdata(CellID,t,gain) ;
}

/* retrieve PhysCaliTdiff ******************************************************
 */
const float& LArPhysCaliTdiffComplete::Tdiff(const HWIdentifier& CellID, int gain) const 
{ 

  const LArPhysCaliTdiffP& t= get(CellID,gain); 

  return t.m_Tdiff;
}


  

