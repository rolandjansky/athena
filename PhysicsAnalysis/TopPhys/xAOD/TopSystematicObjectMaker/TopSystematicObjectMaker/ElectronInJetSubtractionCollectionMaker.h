/*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// $Id: ElectronInJetSubtractionCollectionMaker.h 653798 2015-03-12 19:24:22Z morrisj $
#ifndef ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_ELECTRONINJETSUBTRACTIONCOLLECTIONMAKER_H
#define ANALYSISTOP_TOPSYSTEMATICOBJECTMAKER_ELECTRONINJETSUBTRACTIONCOLLECTIONMAKER_H

/**
  * @author John Morris <john.morris@cern.ch>
  * 
  * @brief ElectronInJetSubtractionCollectionMaker 
  *   Makes all needed collections for the Electron in jet subtraction 
  *   Used by boosted analysis 
  * 
  *   Will require a set of electron and jet collections for:
  *     Nominal
  *     All Electron systematics
  *     All Jet systematics
  * 
  *   This class does not do the actual subtraction algorithm
  *   This class prepares the collections and calls the algorithm
  * 
  * $Revision: 653798 $
  * $Date: 2015-03-12 20:24:22 +0100 (Thu, 12 Mar 2015) $
  * 
  **/ 

// system include(s):
#include <memory>

// Framework include(s):
#include "AsgTools/AsgTool.h"
#include "xAODEgamma/ElectronContainer.h"
#include "xAODJet/JetContainer.h"

// ElectronInJetSubtractionAlgorithm
#include "TopJetSubstructure/ElectronInJetSubtractionAlgorithm.h"

// Forward declaration(s):
namespace top{
  class TopConfig;
}

namespace top{
  
  class ElectronInJetSubtractionCollectionMaker final : public asg::AsgTool {
    public:
      explicit ElectronInJetSubtractionCollectionMaker( const std::string& name );
      virtual ~ElectronInJetSubtractionCollectionMaker(){}
      
      // Delete Standard constructors
      ElectronInJetSubtractionCollectionMaker(const ElectronInJetSubtractionCollectionMaker& rhs) = delete;
      ElectronInJetSubtractionCollectionMaker(ElectronInJetSubtractionCollectionMaker&& rhs) = delete;
      ElectronInJetSubtractionCollectionMaker& operator=(const ElectronInJetSubtractionCollectionMaker& rhs) = delete; 
      
      StatusCode initialize(); 
      StatusCode execute();
      
    private:      
      std::shared_ptr<top::TopConfig> m_config;
      bool m_doLooseCuts;
      // The algorithm
      top::ElectronInJetSubtractionAlgorithm m_subtractionAlgorithm;
  };
}

#endif
