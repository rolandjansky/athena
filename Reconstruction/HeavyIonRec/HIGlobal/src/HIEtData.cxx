/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

///////////////////////////////////////////////////////////////////
// HIEtData.cxx, (c) ATLAS Detector software
///////////////////////////////////////////////////////////////////

#include "HIGlobal/HIEtData.h"
#include "StoreGate/StoreGateSvc.h"
#include "HIGlobal/HICaloUtil.h"
//////////////////////////////////////////////////////////////////
void HIEtData::EtvsEta(float * et_vs_eta)
  {    
	for(int i=0; i<50 ; i++)
      {
        m_Et_final.push_back(et_vs_eta[i]);      
      }    
  }

