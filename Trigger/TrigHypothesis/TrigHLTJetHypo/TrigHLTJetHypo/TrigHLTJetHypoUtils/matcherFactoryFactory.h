/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MATCHERMATCHERFACTORY_H
#define TRIGHLTJETHYPO_MATCHERMATCHERFACTORY_H

// ********************************************************************
//
// NAME:     matcherMatcherFactory.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <memory>
#include <string>
#include <vector>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"


// matcherFactoryFactory functions return IMatcher factory instances.
// The function overloads are used to differntiate the different
// matcher factories to instantiate.
// the extra level of indiretion allows the Factory classes to store
// only the information needed to instantiate their particular Matcher
std::shared_ptr<IMatcherFactory> matcherFactoryFactory(const Conditions&,
						       const std::string&);

std::shared_ptr<IMatcherFactory> matcherFactoryFactory(const Conditions&,
						       const std::vector<unsigned int>,
						       const std::string&);


#endif
