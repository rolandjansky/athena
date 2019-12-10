// this is a -*- C++ -*- file
/*
  Copyright (C) 2002-2019 CERN for the benefit of the ATLAS collaboration
*/

////////////////////////////////////////////////////
/// \class JetRecAlg 
///
/// Algorithm tasked to create a single JetContainer
/// This algorithm makes use of 2 types of tools :
///   - a IJetProvider : used to build "empty" jets (for example : running fastjet on input, grooming an existing JetContainer or copying an existing JetContainer)
///   - a list of IJetModifier : each modifier is run on the created JetContainer to filter, sort and/or add moments to the jets.
///
///

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

  JetRecAlg(const std::string& name, ISvcLocator* pSvcLocator);

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
  ToolHandle<IJetProvider> m_jetprovider ={this , "Builder" , {} , "Tool building the jets (fastjet, copy, grooming...)"};
  ToolHandleArray<IJetModifier> m_modifiers = {this , "Modifiers", {}, "moment calculators" };
  SG::WriteHandleKey<xAOD::JetContainer> m_output= {this, "OutputContainer", "AntiKt4LCtopoJets", "The output jet container name"};
  
}; 

#endif

