/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#include "TriggerMenuAnalysis/CombLinksDef.h"

#include "TrigSteeringEvent/TrigRoiDescriptor.h"

#include "TrigCaloEvent/TrigT2JetContainer.h"
#include "JetEvent/JetCollection.h"

void CombLinksDef::buildKnownCombLinksDefs5()
{
  //----------------------------------------------------------------
  // j
  //----------------------------------------------------------------
  CombLinksDef* x = new CombLinksDef(ChainEntry::kRoIType_L2_j);
  x->addL1RoI("Jet_ROI");
  x->addFeature<TrigT2Jet, TrigT2JetContainer>("HLT_TrigT2CaloJet");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

  x = new CombLinksDef(ChainEntry::kRoIType_EF_j);
  //  x->addL1RoI("Jet_ROI");
  x->addContFeature<JetCollection, Jet>("HLT_TrigJetRec");
  if (addCombLinksDef(x->getRoIType(), x) == false) delete x;

}
