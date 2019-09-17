 /*
  Copyright (C) 2002-2017 CERN for the benefit of the ATLAS collaboration
*/

// JetAlgorithm.h 

#ifndef JetAlgorithm_H
#define JetAlgorithm_H

#include "AthenaBaseComps/AthAlgorithm.h"
#include "GaudiKernel/ToolHandle.h"
#include "StoreGate/WriteHandleKey.h"

#include "JetInterface/IJetProvider.h"
#include "JetInterface/IJetModifier.h"


class IJetExecuteTool;

class JetAlgorithm : public AthAlgorithm { 

public: 

  /// Constructor with parameters: 
  JetAlgorithm(const std::string& name, ISvcLocator* pSvcLocator);

  /// Destructor: 
  ~JetAlgorithm(); 

  /// Athena algorithm's Hooks
  StatusCode  initialize();
  StatusCode  execute();
  StatusCode  finalize();

private: 

  /// Default constructor: 
  JetAlgorithm();

private:

  /// Athena configured tools
  //  ToolHandleArray<IJetExecuteTool> m_exetools;
  ToolHandle<IJetProvider> m_jetprovider ={this , "Builder" , {} , "Tool building the jets (fastjet, copy, grooming...)"};
  ToolHandleArray<IJetModifier> m_modifiers = {this , "Modifiers", {}, "moment calculators" };
  SG::WriteHandleKey<xAOD::JetContainer> m_output= {this, "OutputContainer", "AntiKt4LCtopoJets", "The output jet container name"};
  
}; 

#endif
