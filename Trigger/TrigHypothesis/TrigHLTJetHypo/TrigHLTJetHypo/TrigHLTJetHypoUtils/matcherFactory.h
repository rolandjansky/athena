/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

#ifndef TRIGHLTJETHYPO_MATCHERFACTORY_H
#define TRIGHLTJETHYPO_MATCHERFACTORY_H

// ********************************************************************
//
// NAME:     matcherFactory.h
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:  P Sherwood
//
// ********************************************************************
//

#include <memory>
#include <string>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/ConditionsDefs.h"

// matcherFactory instances return IMatcher instances.

class MaximumBipartiteMatcherFactory: public IMatcherFactory {
 public:
  MaximumBipartiteMatcherFactory(const Conditions&, const std::string&);
  std::shared_ptr<IMatcher> make() const override;
 private:
  Conditions m_conditions;
  std::string m_name;
};


class OrderedCollectionsMatcherFactory: public IMatcherFactory {
 public:
  OrderedCollectionsMatcherFactory(const Conditions&, const std::string&);
  std::shared_ptr<IMatcher> make() const override;
 private:
  Conditions m_conditions;
  std::string m_name;
};


class SelectedJetsMatcherFactory: public IMatcherFactory {
 public:
  SelectedJetsMatcherFactory(const Conditions&, 
			     const std::vector<unsigned int>& indices,
			     const std::string&);

  std::shared_ptr<IMatcher> make() const override;
 private:
  Conditions m_conditions;
  std::vector<unsigned int> m_indices;
  std::string m_name;
};

#endif
