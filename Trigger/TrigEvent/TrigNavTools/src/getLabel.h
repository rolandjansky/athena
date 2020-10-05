// This file's extension implies that it's C, but it's really -*- C++ -*-.
/*
 * Copyright (C) 2002-2020 CERN for the benefit of the ATLAS collaboration.
 */
/**
 * @file TrigNavTools/src/getLabel.h
 * @author scott snyder <snyder@bnl.gov>
 * @date May, 2020
 * @brief Helper function to get a label for a feature.
 *
 * Broken out from the old TrigNavigationSlimmingTool (B. Smith / T. Bold).
 */


#ifndef TRIGNAVTOOLS_GETLABEL_H
#define TRIGNAVTOOLS_GETLABEL_H


#include "TrigNavigation/TriggerElement.h"
#include <string>


namespace HLT {


class NavigationCore;


namespace TrigNavTools {


// Wrapped in a class so we can more easily declare it a friend
// in NavigationCore.
struct SlimmingHelper {
  static
  std::string getLabel (const HLT::NavigationCore& navigation,
                        const HLT::TriggerElement::FeatureAccessHelper& fah);
};


}} // namespace HLT::TrigNavTools


#endif // not TRIGNAVTOOLS_GETLABEL_H
