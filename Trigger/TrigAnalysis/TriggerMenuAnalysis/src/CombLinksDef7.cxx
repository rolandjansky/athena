/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

void CombLinksDef::buildKnownCombLinksDefs7()
{
  //----------------------------------------------------------------
  // xe 
  //----------------------------------------------------------------
  CombLinksDef* x = new CombLinksDef(ChainEntry::kRoIType_L2_xe);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_xe);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  //----------------------------------------------------------------
  // DiMu 
  //----------------------------------------------------------------
  x = new CombLinksDef(ChainEntry::kRoIType_L2_DiMu);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_DiMu);
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;
}
