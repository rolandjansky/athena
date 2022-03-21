
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_IJETSMATCHER_H
#define TRIGHLTJETHYPO_IJETSMATCHER_H
/********************************************************************
 *
 * NAME:     IJetsMatcher.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  PABC for matching gropups of jets with Conditions.
 *********************************************************************/

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IJet.h"
#include "./ConditionsDefs.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/HypoJetDefs.h"
#include <string>

class ITrigJetHypoInfoCollector;
class xAODJetCollector;

class IJetsMatcher{
 public:

  virtual ~IJetsMatcher(){}
  virtual std::optional<bool> match(const HypoJetVector& jv,
				    xAODJetCollector&,
				    const std::unique_ptr<ITrigJetHypoInfoCollector>&,
				    bool debug=false) const = 0;

  virtual bool valid() const  = 0;
  virtual std::string msg() const  = 0;
  
  virtual std::string toString() const = 0;
};

#endif


