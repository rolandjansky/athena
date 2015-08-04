/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_MATCHERFACTORY_H
#define TRIGJETHYPO_MATCHERFACTORY_H

// ********************************************************************
//
// NAME:     matcherFactory.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <memory>
#include <string>

#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // IMatcher

std::shared_ptr<IMatcher> matcherFactory(const Conditions&,
                                         const std::string&);


#endif
