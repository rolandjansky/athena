/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_CLEANERFACTORY_H
#define TRIGJETHYPO_CLEANERFACTORY_H


// ********************************************************************
//
// NAME:     cleanerFactory.h
// PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <memory>
#include <string>
#include "TrigJetHypo/TrigHLTJetHypoUtils/TrigHLTJetHypoUtils.h" // ICleaner

std::shared_ptr<ICleaner> cleanerFactory(double,
                                         double,
                                         double,
                                         const std::string&);
#endif
