/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGJETHYPO_ICLEANER_H
#define TRIGJETHYPO_ICLEANER_H
/********************************************************************
 *
 * NAME:     ICleaner.h
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

class ICleaner{
 public:
  // Abstract interface used by a factory function
  virtual bool operator()(const xAOD::Jet* jet) const = 0;
  virtual std::string toString() const noexcept = 0;
  virtual std::string getName() const noexcept= 0;
};

#endif
