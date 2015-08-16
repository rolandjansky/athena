/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     cleanerFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:   P Sherwood
//
// ********************************************************************
//


#include <stdexcept>
#include <memory>

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // IMatcher
#include "./matcherFactory.h"
#include "./MaximumBipartiteMatcher.h"
#include "./SingleEtaRegionMatcher.h"

std::shared_ptr<IMatcher> 
matcherFactory(const Conditions & conditions,
               const std::string& key){

  using pIM = std::shared_ptr<IMatcher>;

  if (key == "maximumBipartite"){
    return pIM(new MaximumBipartiteMatcher(conditions));

  } else if (key == "singleEtaRegion") {
    return pIM(new SingleEtaRegionMatcher(conditions));

  } else {
     throw std::invalid_argument{"Unknown matcher specified :" + key};

  }
}



