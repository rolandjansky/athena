/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TrigCaloEventTPCnv/TrigT2JetCnv_tlp1.h"
 

TrigT2JetCnv_tlp1::TrigT2JetCnv_tlp1()
{
  // add the "main" base class converter (ie. TrigT2JetCnv)
  addMainTPConverter();              
}

void TrigT2JetCnv_tlp1::setPStorage( TrigT2Jet_tlp1 *storage )
{
  setMainCnvPStorage( &storage->m_T2Jet );
  
}
