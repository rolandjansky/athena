// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

// JetRecAlg.h 

#ifndef JetRecAlg_H
#define JetRecAlg_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include "JetInterface/IJetProvider.h"
#include "JetInterface/IJetModifier.h"


class IJetExecuteTool;

class JetRecAlg : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  JetRecAlg(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~JetRecAlg(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  JetRecAlg();

private:

  /// Athena configured tools
  //  ToolHandleArray<IJetExecuteTool> m_exetools;
  ToolHandle<IJetProvider> m_jetprovider ={this , "Builder" , {} , "Tool building the jets (fastjet, copy, grooming...)"};
  ToolHandleArray<IJetModifier> m_modifiers = {this , "Modifiers", {}, "moment calculators" };
  SG::WriteHandleKey<xAOD::JetContainer> m_output= {this, "OutputContainer", "AntiKt4LCtopoJets", "The output jet container name"};
  
}; 

#endif

