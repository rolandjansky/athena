/*
Copyright (C) 2002-2018 CERN for the benefit of the ATLAS collaboration 
*/

#ifndef TRIGBTAGEMULATIONCHAIN_H
#define TRIGBTAGEMULATIONCHAIN_H

#include "TrigBtagEmulationTool/BaseTrigBtagEmulationChainJetIngredient.h"
#include "TrigBtagEmulationTool/TrigBtagEmulationJet.h"

// EDM
#include "xAODEventInfo/EventInfo.h"
#include "xAODTrigger/JetRoIContainer.h"
#include "xAODJet/JetContainer.h"
#include "xAODJet/JetAttributes.h"
#include "xAODJet/JetAuxContainer.h"
#include "AthContainers/AuxElement.h"

#include "TrigDecisionTool/TrigDecisionTool.h"

#include <iostream>
#include <string>
#include <vector>

namespace Trig {
  
  class TrigBtagEmulationChain {    
  public:

    /// Constructor with signature name 
    TrigBtagEmulationChain(const std::vector<std::string>& chainDefinition, ToolHandle<Trig::TrigDecisionTool>& trigDec);    

    /// Trigger decision ingredient definition
    void addDecisionIngredient(std::string decision);
    void addDecisionIngredient(BaseTrigBtagEmulationChainJetIngredient* decision);

    /// Jet Evaluation
    void evaluate();

    /// Chain evaluation
    bool isPassed();

    // Dump
    void Print();

    // Utilities
    bool hasFeature(std::string feature);

    /// Event cleanup
    void clear();
  
    /// Name
    std::string getName() const;
    // Method for accessing configuration outcome
    bool isCorrectlyConfigured() const;
    bool isAutoConfigured() const;
    std::vector< std::string > retrieveAutoConfiguration() const;

    bool addJet(std::string item,std::vector< TrigBtagEmulationJet* >& jets);

  protected:
    bool parseChainName( std::string,std::vector< BaseTrigBtagEmulationChainJetIngredient* >& );
    std::vector< BaseTrigBtagEmulationChainJetIngredient* > processL1trigger (std::string);
    std::vector< BaseTrigBtagEmulationChainJetIngredient* > processHLTtrigger (std::string);

  private:
    // Chain name
    std::string m_name;

    // Chain selections
    std::vector< std::string > m_ingredientsDecision;
    std::vector< BaseTrigBtagEmulationChainJetIngredient* >  m_ingredientsJet;

    // Trigger decision
    ToolHandle<Trig::TrigDecisionTool>& m_trigDec;

    bool m_correctlyConfigured;
    bool m_autoConfigured;
  };

} //namespace

#endif
