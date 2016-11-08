/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IMATCHERFACTORY_H
#define TRIGHLTJETHYPO_IMATCHERFACTORY_H
/********************************************************************
 *
 * NAME:     MactcherFactory.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include <memory>

class IMatcherFactory{
 public:
  virtual ~IMatcherFactory(){}
  virtual  std::shared_ptr<IMatcher> make() const =0;
};

#endif

