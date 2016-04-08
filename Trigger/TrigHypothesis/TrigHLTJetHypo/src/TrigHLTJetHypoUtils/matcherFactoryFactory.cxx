/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// ********************************************************************
//
// NAME:     matcherFactoryFactory.cxx
// PACKAGE:  Trigger/TrigHypothesis/TrigHLTJetHypo
//
// AUTHOR:   P Sherwood
//
// IMatcherFactoryFactory - returns a Factory that creates a concrete Factory
//                          and returns it as an IMatcherFactory.
// ********************************************************************
//


#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/matcherFactoryFactory.h"
#include "TrigHLTJetHypo/TrigHLTJetHypoUtils/matcherFactory.h"

#include <stdexcept>
#include <memory>
#include <sstream>


std::shared_ptr<IMatcherFactory> 
matcherFactoryFactory(const Conditions & conditions,
		      const std::string& key){

  if (key == "maximumBipartite"){
    IMatcherFactory* imf = new MaximumBipartiteMatcherFactory(conditions, key);
    return std::shared_ptr<IMatcherFactory>(imf);
  } else if (key == "orderedCollections"){
    IMatcherFactory* imf = new OrderedCollectionsMatcherFactory(conditions,
								key);
    return std::shared_ptr<IMatcherFactory>(imf);
  } else {
    std::stringstream ss;
    auto m =  "matcherFactory: unknown key to make a matcher factory" + key;
    throw std::runtime_error(m);
  }
}


std::shared_ptr<IMatcherFactory> 
matcherFactoryFactory(const Conditions & conditions,
		      const std::vector<unsigned int> indices,
		      const std::string& key){

  if (key == "selectedJets"){
    IMatcherFactory* imf = new SelectedJetsMatcherFactory(conditions,
							  indices,
							  key);
    return std::shared_ptr<IMatcherFactory>(imf);
  } else {
    std::stringstream ss;
    ss << "matcherFactory: unknown key to make a matcher factory" 
       << key 
       << '\n';
    
    throw std::runtime_error(ss.str());
  }
}

