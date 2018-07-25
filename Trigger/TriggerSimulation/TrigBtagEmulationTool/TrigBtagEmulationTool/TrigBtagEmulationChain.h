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
    /// Constructors and Destructor
    TrigBtagEmulationChain( MsgStream&,const std::string&,ToolHandle<Trig::TrigDecisionTool>& );
    TrigBtagEmulationChain( MsgStream&,const std::string&,const std::vector< std::vector< std::string > >&,ToolHandle<Trig::TrigDecisionTool>& );
    TrigBtagEmulationChain( const TrigBtagEmulationChain& );
    ~TrigBtagEmulationChain();

    /// Name
    std::string name() const;
    /// Chain Definition
    std::vector< std::vector< std::string > > definition() const;

    /// Chain evaluation 
    bool isPassed();
    // Dump
    void print();
    /// Utilities
    bool hasFeature( const std::string& );
    // Method for accessing configuration outcome
    bool isCorrectlyConfigured() const;
    bool isAutoConfigured() const;

    /// clear
    static void clear();
    /// Jet Evaluation
    static void evaluate();

  protected:
    /// Chain evaluation
    bool isPassed( const std::string& ) const;
    /// Trigger decision ingredient definition
    void addDecisionIngredient(const std::string&);

  protected:
    bool parseChainName();

    bool parseChainName( const std::string& );
    bool processL1trigger( std::vector< std::vector< std::string > >&,const std::string& );
    bool processHLTtrigger( std::vector< std::vector< std::string > >&,const std::string& );

    MsgStream& msg() const;
    MsgStream& msg( const MSG::Level lvl ) const;

  private:
    bool recordChain();
    bool recordChain( const std::string& );
    bool recordChain( std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > );

  private:
    // Chain name
    std::string m_name;
    std::vector< std::vector< std::string > > m_definition;

    bool m_correctlyConfigured;
    bool m_autoConfigured;

    // Trigger decision
    ToolHandle<Trig::TrigDecisionTool>& m_trigDec;
    // Msg Stream
    MsgStream &m_msg;

    // Chains to be retrieved with TDT
    std::vector< std::string > m_ingredientsDecision;

  private:
    static bool hasBeenCleared;
    static bool hasBeenEvaluated;
    // All the defined chains
    static std::map< std::string, std::unique_ptr< BaseTrigBtagEmulationChainJetIngredient > > m_chains;
  };

} //namespace

#endif
