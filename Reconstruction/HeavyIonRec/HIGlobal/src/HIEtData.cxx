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
void HIEtData::EtvsEta(const std::vector<float>& et_vs_eta)
{
  m_Et_final = et_vs_eta;
}


void HIEtData::EtvsEta(std::vector<float>&& et_vs_eta)
{
  m_Et_final = std::move (et_vs_eta);
}
