
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IJETSMATCHERMT_H
#define TRIGHLTJETHYPO_IJETSMATCHERMT_H
/********************************************************************
 *
 * NAME:     IJetsMatcherMT.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  PABC for matching gropups of jets with Conditions.
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ConditionsDefsMT.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class IJetsMatcherMT{
 public:

  virtual ~IJetsMatcherMT(){}
  virtual std::optional<bool> match(const HypoJetCIter&,
				    const HypoJetCIter&,
				    xAODJetCollector&,
				    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
				    bool debug=false) const = 0;
  
  virtual std::string toString() const = 0;
};

#endif


