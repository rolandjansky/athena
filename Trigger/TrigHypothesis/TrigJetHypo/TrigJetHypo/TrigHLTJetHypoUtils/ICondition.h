/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ICONDITION_H
#define TRIGJETHYPO_ICONDITION_H
/********************************************************************
 *
 * NAME:     ICondition.h
 * PACKAGE:  Trigger/TrigHypothesis/TrigJetHypo
 *
 * AUTHOR:   P. Sherwood
 * CREATED:  March 21, 2015
 *           
 *
 *  Headers file for visible classes used by TrigHLTJEtHypo
 *********************************************************************/

#include "xAODJet/Jet.h"
#include <string>


class ICondition {
 public:
  virtual ~ICondition(){}
  virtual bool 
    isSatisfied(const std::vector<const xAOD::IParticle*>&) const = 0;
  virtual bool isSatisfied(const xAOD::IParticle*) const = 0;
  virtual std::string toString() const noexcept = 0;
  virtual double orderingParameter() const noexcept = 0;
};

#endif
