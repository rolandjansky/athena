/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICLEANER_H
#define TRIGHLTJETHYPO_ICLEANER_H
/********************************************************************
 *
 * NAME:     ICleaner.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>

class ICleaner{
 public:
  // Abstract interface used by a factory function
  virtual ~ICleaner(){}
  virtual bool operator()(const pHypoJet&) const = 0;
  virtual std::string toString() const noexcept = 0;
  virtual std::string getName() const noexcept= 0;
};

#endif
