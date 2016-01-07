/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_IMATCHERFACTORY_H
#define TRIGJETHYPO_IMATCHERFACTORY_H
/********************************************************************
 *
 * NAME:     MactcherFactory.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "TrigJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include <memory>

class IMatcherFactory{
 public:
  virtual  std::shared_ptr<IMatcher> make() const =0;
  virtual  ~IMatcherFactory(){};
};

#endif

