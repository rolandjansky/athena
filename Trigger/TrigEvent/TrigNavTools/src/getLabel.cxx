/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrigNavTools/src/getLabel.cxx
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2020
 * @brief Helper function to get a label for a feature.
 *
 * Broken out from the old TrigNavigationSlimmingTool (B. Smith / T. Bold).
 */


#include "getLabel.h"
#include "TrigNavigation/Navigation.h"


namespace HLT {
namespace TrigNavTools {


std::string
SlimmingHelper::getLabel(const HLT::NavigationCore& navigation,
                         const HLT::TriggerElement::FeatureAccessHelper& fah)
{
  CLID clid = fah.getCLID();
  std::string label;
  HLTNavDetails::IHolder *h = navigation.getHolder(clid, fah.getIndex().subTypeIndex());
  if(h) label = h->label();
    
  return label;
}


}} // namespace HLT::TrigNavTools
