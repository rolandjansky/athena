/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_ICLEANEROBJECT_H
#define TRIGHLTJETHYPO_ICLEANEROBJECT_H

/********************************************************************
 *
 * NAME:     ICleanerObject.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  October 13, 2016
 *           
 *
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>

// returns a pair of constant iters into a jet collection given a pair of constant iters

class ICleanerObject{
 public:
  virtual ~ICleanerObject(){}
  virtual pairHypoJetIter operator()(const pairHypoJetIter&) const = 0;
  virtual std::string toString() const noexcept = 0;
  virtual std::string getName() const noexcept= 0;
};

#endif
