/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     matcherFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// IMatcherFactoryFactory - returns a Factory that creates a concrete Factory
//                          and returns it as an IMatcherFactory.
// IMatcherFactory - returns a Matcher as an IMatcher.The IMatcherFactory will
//                   be used each event to create a fresh  IMatcher
// MaximumBipartiteMatcher - concrete matcher type
// OrderedCollectionsMatcher - concrete matcher type
// SelectedJetsMatcher - concrete matcher type
// ********************************************************************
//


#include <stdexcept>
#include <sstream>
#include <memory>

#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/IMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/matcherFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/MaximumBipartiteMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/OrderedCollectionsMatcher.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/SelectedJetsMatcher.h"

using pIM = std::shared_ptr<IMatcher>;

MaximumBipartiteMatcherFactory::MaximumBipartiteMatcherFactory(const Conditions & conditions,
							       const std::string& key): 
  m_conditions(conditions), m_name(key){
}

pIM MaximumBipartiteMatcherFactory::make() const { 
  return pIM(new MaximumBipartiteMatcher(m_conditions, m_name));
}

OrderedCollectionsMatcherFactory::OrderedCollectionsMatcherFactory(const Conditions & conditions,
								   const std::string& key): 
  m_conditions(conditions), m_name(key){
}

pIM OrderedCollectionsMatcherFactory::make() const { 
  return pIM(new OrderedCollectionsMatcher(m_conditions, m_name));
}

SelectedJetsMatcherFactory::SelectedJetsMatcherFactory(const Conditions& conditions,
						       const std::vector<unsigned int>& indices,
						       const std::string& key) : 
  m_conditions(conditions), m_indices(indices), m_name(key){
}

pIM SelectedJetsMatcherFactory::make() const { 
  return pIM(new SelectedJetsMatcher(m_conditions, m_indices, m_name));
}
